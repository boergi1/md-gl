
import json
import torch
from torchvision import transforms
from torchvision import models
from PIL import Image


# Specify image transformations
# Line [1]: Here we are defining a variable transform which is a combination of all the image transformations to be carried out on the input image.
# Line [2]: Resize the image to 256×256 pixels.
# Line [3]: Crop the image to 224×224 pixels about the center.
# Line [4]: Convert the image to PyTorch Tensor data type.
# Line [5-7]: Normalize the image by setting its mean and standard deviation to the specified values.
transform = transforms.Compose([            #[1]
transforms.Resize(256),                    #[2]
transforms.CenterCrop(224),                #[3]
transforms.ToTensor(),                     #[4]
transforms.Normalize(                      #[5]
mean=[0.485, 0.456, 0.406],                #[6]
std=[0.229, 0.224, 0.225]                  #[7]
)])


img = Image.open("/home/boergi/orange.jpg")
#img.show()

img_t = transform(img)
batch_t = torch.unsqueeze(img_t, 0)



# Print the models available in torchvision
#print(dir(models))
# Load network model
alexnet = models.alexnet(pretrained=True)
#print(alexnet)

# Put our model in eval mode
alexnet.eval()

# Carry out inference
out = alexnet(batch_t)
print(out.shape)

sorted, indices = torch.sort(out, descending=True)
percentage = torch.nn.functional.softmax(out, dim=1)[0] * 100

# Load labels

classes = json.load(open("imagenet_class_index.json"))


classLabels = [classes[str(k)][1] for k in range(len(classes))]


#for idx in out[0].sort()[1][-10:]:
#    print(classLabels[idx])

print("AlexNet result:")
[print(classLabels[idx], percentage[idx].item()) for idx in indices[0][:5]]

print("ResNet101 result:")
resnet = models.resnet101(pretrained=True)
resnet.eval()
out = resnet(batch_t)
sorted, indices = torch.sort(out, descending=True)
percentage = torch.nn.functional.softmax(out, dim=1)[0] * 100
[print(classLabels[idx], percentage[idx].item()) for idx in indices[0][:5]]

