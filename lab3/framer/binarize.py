import cv2
import os
import numpy as np

def binarize_video(video_path, output_folder, threshold=127):
    """
    Binarize a grayscale video and save each frame as a true 1-bit depth image
    by packing 8 pixels into each byte.
    
    Args:
        video_path (str): Path to the input video file
        output_folder (str): Path to folder where frames will be saved
        threshold (int): Threshold value for binarization (0-255)
    """
    # Create output folder if it doesn't exist
    if not os.path.exists(output_folder):
        os.makedirs(output_folder)
    
    # Open the video file
    cap = cv2.VideoCapture(video_path)
    
    # Check if video opened successfully
    if not cap.isOpened():
        print(f"Error: Could not open video {video_path}")
        return False
    
    frame_count = 0
    
    while True:
        # Read the next frame
        ret, frame = cap.read()
        
        # Break if end of video
        if not ret:
            break
        
        # Convert to grayscale if it's not already
        if len(frame.shape) == 3:
            gray = cv2.cvtColor(frame, cv2.COLOR_BGR2GRAY)
        else:
            gray = frame
        
        # Apply thresholding to binarize the image (0 for black, 1 for white)
        _, binary = cv2.threshold(gray, threshold, 1, cv2.THRESH_BINARY)

        
        # Save the packed binary data
        raw_filename = os.path.join(output_folder, f"frame_{frame_count:06d}.bin")
        with open(raw_filename, 'wb') as f:
            f.write(binary)
        
        frame_count += 1
    
    # Release the video capture object
    cap.release()
    
    print(f"Successfully processed {frame_count} frames")
    return True

def reconstruct_binary_image(bin_file, meta_file):
    """
    Reconstruct a binary image from a packed 1-bit file and its metadata.
    Useful for verification or visualization.
    
    Args:
        bin_file (str): Path to the binary file
        meta_file (str): Path to the metadata file with dimensions
    
    Returns:
        numpy.ndarray: Reconstructed binary image
    """
    # Read metadata
    with open(meta_file, 'r') as f:
        lines = f.readlines()
        width = int(lines[0].split(': ')[1])
        height = int(lines[1].split(': ')[1])
    
    # Read binary data
    with open(bin_file, 'rb') as f:
        packed_data = bytearray(f.read())
    
    # Calculate bytes per row
    bytes_per_row = (width + 7) // 8
    
    # Create the binary image
    binary_image = np.zeros((height, width), dtype=np.uint8)
    
    # Unpack the bytes
    for y in range(height):
        for byte_x in range(bytes_per_row):
            if y * bytes_per_row + byte_x < len(packed_data):
                byte_val = packed_data[y * bytes_per_row + byte_x]
                for bit_pos in range(8):
                    x = byte_x * 8 + bit_pos
                    if x < width:
                        # Extract each bit (from MSB to LSB)
                        bit = (byte_val >> (7 - bit_pos)) & 1
                        binary_image[y, x] = bit * 255  # Scale back to 0-255 for visualization
    
    return binary_image

if __name__ == "__main__":
    # Example usage
    video_path = "scaled.mp4"  # Replace with your video path
    output_folder = "frames"
    threshold = 127  # Adjust based on your needs
    
    binarize_video(video_path, output_folder, threshold)
    
    # Uncomment the following to test reconstruction
    """
    frame_num = 0
    bin_file = os.path.join(output_folder, f"frame_{frame_num:06d}.bin")
    meta_file = os.path.join(output_folder, f"frame_{frame_num:06d}.txt")
    
    if os.path.exists(bin_file) and os.path.exists(meta_file):
        reconstructed = reconstruct_binary_image(bin_file, meta_file)
        cv2.imwrite("reconstructed.png", reconstructed)
        print("Saved reconstructed image for verification")
    """