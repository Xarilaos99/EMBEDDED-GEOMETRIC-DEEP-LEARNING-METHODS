import cv2
import numpy as np
import matplotlib.pyplot as plt
import argparse


# write the path of the diploma_quant\image0.txt file here 
image_path=r'...\workspace\diploma_quant\image0.txt'


parser = argparse.ArgumentParser(description='A simple script with a command-line argument.')

# Add an argument
# parser.add_argument('myarg', type=int, help='Path to the input file')
parser.add_argument('myarg', type= int, nargs='*',default=[0,0], help='A numeric value (default: 0)')

args = parser.parse_args()

num=args.myarg

with open(image_path, 'r') as file:
    data = [value for line in file for value in line.split(":")[1].split()]
    # data = [value for line in file for value in line.split()]

    # data = file.read().split('\n')



print(len(data))

data = [int(hex_value, 16) for hex_value in data]
data = np.array(data, dtype=np.uint8)
# resized_image = cv2.resize(data, (28, 28), interpolation=cv2.INTER_LINEAR)
reshaped_arr = data.reshape(28, 28)


cv2.imwrite("DataSet/Image" + str(num[0]) +".png", reshaped_arr)




with open("DataSet/Number/Image" + str(num[0]) +".txt", "w") as file:
    file.write(str(num[1]))




cv2.imshow('Image', reshaped_arr.astype(np.uint8))
cv2.waitKey(0)
cv2.destroyAllWindows()
