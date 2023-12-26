from math import sqrt

import torch
import torch.nn.functional as F
from torch import nn

import ai8x
import utils.object_detection_utils as obj_detect_utils


class TinySSDBase(nn.Module):

    def __init__(self, **kwargs):
        super().__init__()
        
        # Standard convolutional layers
        self.fire1 = ai8x.FusedConv2dBNReLU(3, 32, 3, padding=1, **kwargs)
        self.fire2 = ai8x.FusedConv2dBNReLU(32, 32, 3, padding=1, **kwargs)

        self.fire3 = ai8x.FusedMaxPoolConv2dBNReLU(32, 64, 3, padding=1, **kwargs)
        self.fire4 = ai8x.FusedConv2dBNReLU(64, 64, 3, padding=1, **kwargs)

        self.fire5 = ai8x.FusedMaxPoolConv2dBNReLU(64, 64, 3, padding=1,
                                                   pool_size=3, **kwargs)
        self.fire6 = ai8x.FusedConv2dBNReLU(64, 64, 3, padding=1, **kwargs)
        self.fire7 = ai8x.FusedConv2dBNReLU(64, 128, 3, padding=1, **kwargs)
        self.fire8 = ai8x.FusedConv2dBNReLU(128, 32, 3, padding=1, **kwargs)

        self.fire9 = ai8x.FusedMaxPoolConv2dBNReLU(32, 32, 3, padding=1,
                                                   **kwargs)

        self.fire10 = ai8x.FusedMaxPoolConv2dBNReLU(32, 32, 3, padding=1,
                                                    pool_size=3, **kwargs)

    def forward(self, image):

        out = self.fire1(image)                     # (N, 32, 50, 60)                
        out = self.fire2(out)                       # (N, 32, 50, 60)

        out = self.fire3(out)                       # (N, 64, 50, 60)
        fire4_feats = self.fire4(out)               # (N, 64, 50, 60)           

        out = self.fire5(fire4_feats)               # (N, 64, 25, 30)            
        out = self.fire6(out)                       # (N, 64, 25, 30)
        out = self.fire7(out)                       # (N, 128, 25, 30)
        fire8_feats = self.fire8(out)               # (N, 32, 25, 30)        

        fire9_feats = self.fire9(fire8_feats)       # (N, 32, 12, 15)     

        fire10_feats = self.fire10(fire9_feats)     # (N, 32, 6, 7)   

        return fire4_feats, fire8_feats, fire9_feats, fire10_feats 


class AuxiliaryConvolutions(nn.Module):

    def __init__(self, **kwargs):
        super().__init__()

        # Auxiliary/additional convolutions on top of the VGG base
        self.conv12_1 = ai8x.FusedConv2dBNReLU(32, 16, 3, padding=1, **kwargs)           # (N, 16, 6, 7)
        self.conv12_2 = ai8x.FusedMaxPoolConv2dBNReLU(16, 16, 3, padding=1, **kwargs)    # (N, 16, 3, 3)

        self.init_conv2d()

    def init_conv2d(self):
    
        for c in self.children():
            if isinstance(c, nn.Conv2d):
                nn.init.xavier_uniform_(c.weight)
                if c.bias:
                    nn.init.constant_(c.bias, 0.)

    def forward(self, fire10_feats):
    
        out = self.conv12_1(fire10_feats)
        conv12_2_feats = self.conv12_2(out)

        return conv12_2_feats


class PredictionConvolutions(nn.Module):

    def __init__(self, n_classes, **kwargs):

        super().__init__()

        self.n_classes = n_classes
        
        n_boxes = {'fire8': 4,
#                   'fire9': 4,
#                   'fire10': 2,
                   'conv12_2': 4}

        # 4 prior-boxes implies we use 4 different aspect ratios, etc.
        self.loc_fire8 = ai8x.FusedConv2dBN(32, n_boxes['fire8'] * 4, kernel_size=3, padding=1,    #产生 n_boxes['fire8'] * 4 个输出通道
                                            **kwargs)
        
        self.loc_conv12_2 = ai8x.FusedConv2dBN(16, n_boxes['conv12_2'] * 4, kernel_size=3,
                                               padding=1, **kwargs)

        # Class prediction convolutions (predict classes in localization boxes)
        self.cl_fire8 = ai8x.FusedConv2dBN(32, n_boxes['fire8'] * n_classes, kernel_size=3,
                                           padding=1, **kwargs)

        self.cl_conv12_2 = ai8x.FusedConv2dBN(16, n_boxes['conv12_2'] * n_classes, kernel_size=3,
                                              padding=1, **kwargs)

        # Initialize convolutions' parameters
        self.init_conv2d()

    def init_conv2d(self):

        for c in self.children():
            if isinstance(c, nn.Conv2d):
                nn.init.xavier_uniform_(c.weight)
                if c.bias:
                    nn.init.constant_(c.bias, 0.)

    def forward(self, fire4_feats, fire8_feats, conv12_2_feats):

        batch_size = fire4_feats.size(0)
    

        l_fire8 = self.loc_fire8(fire8_feats)
        l_fire8 = l_fire8.permute(0, 2, 3, 1).contiguous()
        l_fire8 = l_fire8.view(batch_size, -1, 4)


        l_conv12_2 = self.loc_conv12_2(conv12_2_feats)
        l_conv12_2 = l_conv12_2.permute(0, 2, 3, 1).contiguous()
        l_conv12_2 = l_conv12_2.view(batch_size, -1, 4)


        c_fire8 = self.cl_fire8(fire8_feats)
        c_fire8 = c_fire8.permute(0, 2, 3, 1).contiguous()
        c_fire8 = c_fire8.view(batch_size, -1, self.n_classes)
        

        c_conv12_2 = self.cl_conv12_2(conv12_2_feats)
        c_conv12_2 = c_conv12_2.permute(0, 2, 3, 1).contiguous()
        c_conv12_2 = c_conv12_2.view(batch_size, -1, self.n_classes)

        # Concatenate in this specific order (i.e. must match the order of the prior-boxes)
     
        locs = torch.cat([ l_fire8, l_conv12_2], dim=1)
        classes_scores = torch.cat([ c_fire8,  c_conv12_2],
                                   dim=1)
        

#        print("类别分数的长度" , len(classes_scores))
#        print("类别分数的维度" , classes_scores.shape)           
        return (locs, classes_scores)  

class MY_TinierSSD(nn.Module):
    
    default_aspect_ratios = (
        (0.95, 0.6, 0.4, 0.25),
        (0.95, 0.6, 0.4, 0.25),
    )

    def __init__(self, num_classes,                      
                 num_channels=3,                         
                 dimensions=(74, 74),                    
                 aspect_ratios=default_aspect_ratios,    
                 device='cpu',
                 **kwargs):
        super().__init__()    
        
#        print("类别数量", num_classes)        
        
        self.n_classes = num_classes

        self.base = TinySSDBase(**kwargs)
        self.aux_convs = AuxiliaryConvolutions(**kwargs)
        self.pred_convs = PredictionConvolutions(self.n_classes, **kwargs)


        self.device = device
        self.priors_cxcy = self.__class__.create_prior_boxes(aspect_ratios=aspect_ratios,
                                                             device=self.device)


    def forward(self, image):

#        print("模型图片输入维度", image.shape)
        
        fire4_feats, fire8_feats, fire9_feats, fire10_feats = self.base(image)

        conv12_2_feats = self.aux_convs(fire10_feats)

        locs, classes_scores = self.pred_convs(fire4_feats, fire8_feats, conv12_2_feats)

        return locs, classes_scores
 

    def create_prior_boxes(aspect_ratios=default_aspect_ratios, device='cpu'):    

        fmap_dims = {
                     'fire8': 18,
                     'conv12_2': 2}
                     
        fmaps = list(fmap_dims.keys())

        obj_scales = {
                      'fire8': 0.15,
                      'conv12_2': 0.26}

        if len(aspect_ratios) != len(fmaps):
            raise ValueError(f'aspect_ratios list should have length {len(fmaps)}')

        if True in (len(aspect_ratios_list) !=
                    len(MY_TinierSSD.default_aspect_ratios[0])
                    for aspect_ratios_list in aspect_ratios):
            raise ValueError(f'Each aspect_ratios list should have length \
                               {len(MY_TinierSSD.default_aspect_ratios[0])}') 
            
        aspect_ratios = {
                         'fire8': aspect_ratios[0],
                         'conv12_2': aspect_ratios[1]}

        prior_boxes = []
        
        for k, fmap in enumerate(fmaps):
            for i in range(fmap_dims[fmap]):
                for j in range(fmap_dims[fmap]):
                    cx = (j + 0.5) / fmap_dims[fmap]
                    cy = (i + 0.5) / fmap_dims[fmap]

                    for ratio in aspect_ratios[fmap]:
                        prior_boxes.append([cx, cy, obj_scales[fmap] * sqrt(ratio),
                                            obj_scales[fmap] / sqrt(ratio)])


                        if ratio == 1.:
                            try:
                                additional_scale = sqrt(obj_scales[fmap] *
                                                        obj_scales[fmaps[k + 1]])
   
                            except IndexError:
                                additional_scale = 1.
                            prior_boxes.append([cx, cy, additional_scale, additional_scale])


        prior_boxes = torch.FloatTensor(prior_boxes).to(device)  
        prior_boxes.clamp_(0, 1)  

        print("预测框数量", len(prior_boxes))
        return prior_boxes


    def detect_objects(self, predicted_locs, predicted_scores, min_score, max_overlap, top_k):

        batch_size = predicted_locs.size(0)
        n_priors = self.priors_cxcy.size(0)
        predicted_scores = F.softmax(predicted_scores, dim=2)

        # Lists to store final predicted boxes, labels, and scores for all images
        all_images_boxes = []
        all_images_labels = []
        all_images_scores = []
        

        assert n_priors == predicted_locs.size(1) == predicted_scores.size(1)

        for i in range(batch_size):
            # Decode object coordinates from the form we regressed predicted boxes to
            decoded_locs = obj_detect_utils.cxcy_to_xy(
                obj_detect_utils.gcxgcy_to_cxcy(predicted_locs[i], self.priors_cxcy))

            # Lists to store boxes and scores for this image
            image_boxes = []
            image_labels = []
            image_scores = []

            # Check for each class
            for c in range(1, self.n_classes):
                # Keep only predicted boxes and scores where scores for this class are above the
                # minimum score
                class_scores = predicted_scores[i][:, c]
                score_above_min_score = class_scores > min_score
                n_above_min_score = score_above_min_score.sum().item()
                if n_above_min_score == 0:
                    continue
                class_scores = class_scores[score_above_min_score]
                class_decoded_locs = decoded_locs[score_above_min_score]  # (n_qualified, 4)

                # Sort predicted boxes and scores by scores
                class_scores, sort_ind = class_scores.sort(dim=0, descending=True)
                # (n_qualified), (n_min_score)
                class_decoded_locs = class_decoded_locs[sort_ind]  # (n_min_score, 4)

                # Find the overlap between predicted boxes
                overlap = obj_detect_utils.find_jaccard_overlap(class_decoded_locs,
                                                                class_decoded_locs)
                # (n_qualified, n_min_score)

                # Non-Maximum Suppression (NMS)

                # A torch.bool tensor to keep track of which predicted boxes to suppress
                # True implies suppress, False implies don't suppress
                suppress = torch.zeros((n_above_min_score), dtype=torch.bool).to(self.device)
                # (n_qualified)

                # Consider each box in order of decreasing scores
                for box in range(class_decoded_locs.size(0)):
                    # If this box is already marked for suppression
                    if suppress[box]:
                        continue

                    # Suppress boxes whose overlaps (with this box) are greater than maximum
                    # overlap
                    # Find such boxes and update suppress indices
                    suppress = torch.logical_or(suppress, overlap[box] > max_overlap)
                    # The max operation retains previously suppressed boxes, like an 'OR' operation

                    # Don't suppress this box, even though it has an overlap of 1 with itself
                    suppress[box] = False

                # Store only unsuppressed boxes for this class
                image_boxes.append(class_decoded_locs[~suppress])
                image_labels.append(
                    torch.LongTensor((~suppress).sum().item() * [c]).to(self.device))
                image_scores.append(class_scores[~suppress])

            # If no object in any class is found, store a placeholder for 'background'
            if len(image_boxes) == 0:
                image_boxes.append(torch.FloatTensor([[0., 0., 1., 1.]]).to(self.device))
                image_labels.append(torch.LongTensor([0]).to(self.device))
                image_scores.append(torch.FloatTensor([0.]).to(self.device))

            # Concatenate into single tensors
            image_boxes = torch.cat(image_boxes, dim=0)  # (n_objects, 4)
            image_labels = torch.cat(image_labels, dim=0)  # (n_objects)
            image_scores = torch.cat(image_scores, dim=0)  # (n_objects)
            n_objects = image_scores.size(0)

            # Keep only the top k objects
            if n_objects > top_k:
                image_scores, sort_ind = image_scores.sort(dim=0, descending=True)
                image_scores = image_scores[:top_k]  # (top_k)
                image_boxes = image_boxes[sort_ind][:top_k]  # (top_k, 4)
                image_labels = image_labels[sort_ind][:top_k]  # (top_k)

            # Append to lists that store predicted boxes and scores for all images
            all_images_boxes.append(image_boxes)
            all_images_labels.append(image_labels)
            all_images_scores.append(image_scores)

        return all_images_boxes, all_images_labels, all_images_scores  # lists of length batch_size
        
        
def my_ssd_test_28(pretrained=False, **kwargs):
    """
    Constructs a Tinier SSD model
    """
    assert not pretrained
    return MY_TinierSSD(aspect_ratios=MY_TinierSSD.default_aspect_ratios, **kwargs)


models = [
    {
        'name': 'my_ssd_test_28',
        'min_input': 1,
        'dim': 2,
    }
]

