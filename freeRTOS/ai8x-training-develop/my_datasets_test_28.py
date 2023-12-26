import ast
import errno
import os
import pickle
import random
import sys

import numpy as np
import torch
from torch.utils.data import Dataset
from torchvision import transforms

import h5py
import pandas as pd
from PIL import Image
import matplotlib.pyplot as plt

import ai8x

class MY_DATASETS(Dataset):
    def __init__(self, root_dir, d_type, transform=None, resize_size=(74, 74)): 
        
        if d_type not in ('test', 'train'):
            raise ValueError("d_type can only be set to 'test' or 'train'")
            
        if resize_size[0] != 74:
            raise ValueError('Input size error')
            
        if resize_size[1] != 74:
            raise ValueError('Input size error')
            
        self.root_dir = root_dir            
        self.d_type = d_type
        self.transform = transform
        self.resize_size = resize_size
        self.info_df = pd.DataFrame() 
        
        self.img_list = []
        self.boxes_list = []
        self.lbls_list = []
        
        self.processed_folder = os.path.join(root_dir, self.__class__.__name__,'processed')
        self.__makedir_exist_ok(self.processed_folder)
               
        res_string = str(self.resize_size[0]) + 'x' + str(self.resize_size[1])

        train_pkl_file_path = os.path.join(self.processed_folder, 'train_' + res_string + '_fold_'  + '.pkl')
        test_pkl_file_path = os.path.join(self.processed_folder, 'test_' + res_string + '_fold_'  + '.pkl')
        
        if self.d_type == 'train':
            self.pkl_file = train_pkl_file_path
            self.info_df_csv_file = os.path.join(self.processed_folder, 'train_info.csv')
        elif self.d_type == 'test':
            self.pkl_file = test_pkl_file_path
            self.info_df_csv_file = os.path.join(self.processed_folder, 'test_info.csv')
        else:
            print(f'Unknown data type: {self.d_type}')
            return   
    
        self.__create_info_df_csv()
        self.__create_pkl_file()  

    def __create_info_df_csv(self):

        if os.path.exists(self.info_df_csv_file):
            self.info_df = pd.read_csv(self.info_df_csv_file)
        
            for column in self.info_df.columns:
                if column in ['label', 'x0', 'x1', 'y0', 'y1']:
                    self.info_df[column] = \
                        self.info_df[column].apply(ast.literal_eval)
        else:             
            print("worry")
 
    def __create_pkl_file(self):
        if os.path.exists(self.pkl_file):
            (self.img_list, self.boxes_list, self.lbls_list) = \
                    pickle.load(open(self.pkl_file, 'rb'))     
            
            return
        self.__gen_datasets()
        
    def __gen_datasets(self):

        for _, row in self.info_df.iterrows():
            image = Image.open(os.path.join(self.root_dir, self.__class__.__name__, self.d_type,
                                                row['img_name']))   
        
            self.img_list.append(image)  
                        
            boxes = []
            for b in range(len(row['x0'])):
            
                x0_new = row['x0'][b] 
                y0_new = row['y0'][b] 
                x1_new = row['x1'][b] 
                y1_new = row['y1'][b] 
                
                boxes.append([x0_new, y0_new, x1_new, y1_new])
            
            self.boxes_list.append(boxes)
            
            lbls = row['label']
            self.lbls_list.append(lbls)
               
        pickle.dump((self.img_list, self.boxes_list, self.lbls_list), open(self.pkl_file, 'wb'))
              
    def __len__(self): 
        
        return len(self.img_list) 


    def __getitem__(self, index):
        
#        index = 0
        if torch.is_tensor(index):
            index = index.tolist()   

        transform = transforms.Compose([
            transforms.ToTensor(),
        ])
        
        img = self.img_list[index]          
        boxes = self.boxes_list[index]      
        lbls = self.lbls_list[index]        

        img = self.__normalize_image(img).astype(np.float32)        

        if self.transform is not None:
            img = self.transform(img)

            # Normalize boxes:
            boxes = [[box_coord / self.resize_size[0] for box_coord in box] for box in boxes]
            boxes = torch.as_tensor(boxes, dtype=torch.float32)
            labels = torch.as_tensor(lbls, dtype=torch.int64)
            
#        print("getitem的图片", img.shape)        
        return img,  (boxes, labels)
    
    def __makedir_exist_ok(self, dirpath):
        try:
            os.makedirs(dirpath)
        except OSError as e:
            if e.errno == errno.EEXIST:
                pass
            else:
                raise

    def __normalize_image(self, image):
        image = np.array(image)
        return image / 256
    
    def collate_fn(batch):

        images = []
        boxes_and_labels = []

        for b in batch:
            images.append(b[0])
            boxes_and_labels.append(b[1])

        images = torch.stack(images, dim=0)
                
        return images, boxes_and_labels  
    
def get_my_datasets(data, load_train=True, load_test=True, resize_size=(74, 74)):

    (data_dir, args) = data
        
    if load_train:
        train_transform = transforms.Compose([
            transforms.ToTensor(),
            ai8x.normalize(args=args)  #if args is not None else transforms.Normalize((0.5, 0.5, 0.5), (0.5, 0.5, 0.5))
        ])
        
        train_dataset = MY_DATASETS(root_dir=data_dir, d_type='train',
                             transform=train_transform, resize_size=resize_size)     
    else:
        train_dataset = None
        
    if load_test:
        test_transform = transforms.Compose([
            transforms.ToTensor(),
            ai8x.normalize(args=args)  #if args is not None else transforms.Normalize((0.5, 0.5, 0.5), (0.5, 0.5, 0.5))
        ])
        
        test_dataset = MY_DATASETS(root_dir=data_dir, d_type='test',
                             transform=test_transform, resize_size=resize_size)   
    else:
        test_dataset = None
        
    return train_dataset, test_dataset

def get_my_datasets_ls(data, load_train=True, load_test=True):

    return get_my_datasets(data, load_train, load_test, resize_size=(74, 74))    
    
datasets = [
   {
       'name': 'MY_DATASETS_28',
       'input': (3, 74, 74),
       'output': (1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11),
       'loader': get_my_datasets_ls,
       'collate': MY_DATASETS.collate_fn
   }
]