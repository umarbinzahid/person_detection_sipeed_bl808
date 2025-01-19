'''
python script.py input.jpg output.c
'''

import argparse
from PIL import Image
import numpy as np

def process_image(input_file, output_file):
    # Open the original image
    image = Image.open(input_file)

    # Convert the image to greyscale
    greyscale_image = image.convert("L")

    # Resize the image to 96x96 resolution
    resized_image = greyscale_image.resize((96, 96))

    # Convert the image to a numpy array (1D array of pixel values)
    image_data = np.array(resized_image)

    # Flatten the 2D array into a 1D array
    flat_data = image_data.flatten()

    # Open the C file to write the data
    with open(output_file, "w") as c_file:
        c_file.write("#include \"test_image_data.h\"\n\n")
        c_file.write("const unsigned int g_test_image_data_size = 9216;\n\n")
        c_file.write("__attribute__((aligned(16))) const unsigned char g_test_image_data[] = {\n")
        
        # Write the flattened data in the C array format
        for i, byte in enumerate(flat_data):
            if i % 12 == 0 and i != 0:  # Start a new line every 12 bytes for readability
                c_file.write("\n")
            c_file.write(f"0x{byte:02x}, ")
        
        c_file.write("\n};\n")

    print(f"The image has been processed and saved in '{output_file}'.")

if __name__ == "__main__":
    # Create argument parser
    parser = argparse.ArgumentParser(description="Convert a JPEG image to greyscale, resize it, and save it as a C array.")
    parser.add_argument("input_file", help="Input JPEG file path")
    parser.add_argument("output_file", help="Output C file path to save the array")

    # Parse the arguments
    args = parser.parse_args()

    # Process the image
    process_image(args.input_file, args.output_file)
