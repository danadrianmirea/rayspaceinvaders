from PIL import Image
import numpy as np
import matplotlib.pyplot as plt

# Load the image
input_path = "input.png"  # Replace with your input path
output_path = "obstacle.png"

# Open image and convert to RGBA
img = Image.open(input_path).convert("RGBA")
img_array = np.array(img)

# Define the yellow RGB value to match
yellow_rgb = (242, 215, 65)

base_color = (255, 255, 255, 255)
gradient_steps = 16

# Generate darker variants of the base color
def darken_color(color, factor):
    r, g, b, a = color
    return (int(r * factor), int(g * factor), int(b * factor), a)

# Create gradient levels with evenly spaced darkening factors
step_size = 0.9 / (gradient_steps - 1)  # 0.9 is the total range (from 1.0 to 0.1)
shield_colors = [
    darken_color(base_color, 1.0 - (i * step_size)) for i in range(gradient_steps)
]

# Apply gradient to yellow pixels
height = img_array.shape[0]
for y in range(height):
    # Calculate the gradient index based on the y position
    gradient_index = int((y / height) * (gradient_steps - 1))
    color = shield_colors[gradient_index]
    for x in range(img_array.shape[1]):
        r, g, b, a = img_array[y, x]
        if (r, g, b) == yellow_rgb:
            img_array[y, x] = color

# Convert back to image and save
modified_img = Image.fromarray(img_array)
modified_img.save(output_path)

# Optionally display the image
plt.imshow(modified_img)
plt.title("Updated Shield Pixel Art")
plt.axis("off")
plt.show()
