from flask import Flask, request, jsonify
import os
import numpy as np
import face_recognition
from PIL import Image
import logging
import time  # For adding delay if necessary

app = Flask(__name__)

# Set up logging
logging.basicConfig(level=logging.INFO)

# Directory to save images
save_directory = "/Users/rahmansatya/IoT_NUS/application_data/input_image"
if not os.path.exists(save_directory):
    os.makedirs(save_directory)
    logging.info(f"Created directory: {save_directory}")

# Global variable for detection status
facial_result = "0"  # 1 if any match is found, otherwise 0

# Function to get the next image filename with an incremented number
def get_next_image_filename():
    files = [f for f in os.listdir(save_directory) if f.startswith("image_") and f.endswith(".jpg")]
    numbers = [int(f.split("_")[1].split(".")[0]) for f in files if f.split("_")[1].split(".")[0].isdigit()]
    next_number = max(numbers) + 1 if numbers else 1
    return f"{save_directory}/image_{next_number}.jpg"

@app.route('/upload', methods=['POST'])
def upload_image():
    global facial_result
    facial_result = "0"  # Reset for each new request

    try:
        if not request.data:
            logging.error("No data received in request.")
            return jsonify({'error': 'No data received'}), 400
        
        # Save new image
        image_data = request.data
        image_filename = get_next_image_filename()
        
        with open(image_filename, 'wb') as f:
            f.write(image_data)
        logging.info(f"Image saved as: {image_filename}")

        # Introduce a small delay to ensure the file is fully written to disk (if needed)
        time.sleep(0.1)

        # Process the new image
        img = Image.open(image_filename)
        img.save(image_filename)
        logging.info(f"Image flipped vertically and saved as: {image_filename}")

        # Load and encode the uploaded image
        input_img = face_recognition.load_image_file(image_filename)
        input_encodings = face_recognition.face_encodings(input_img)

        if len(input_encodings) == 0:
            logging.warning("No faces found in the input image.")
            return jsonify({'error': 'No faces found in the input image'}), 400

        input_encoding = input_encodings[0]

        # Folder containing images to verify
        verification_folder = "application_data/ver"
        results = []
        detection = 0
        threshold = 0.6  # Match threshold

        if not os.path.exists(verification_folder):
            logging.error(f"Verification images path not found: {verification_folder}")
            return jsonify({'error': 'Verification images path not found'}), 500

        # Process verification images
        for filename in os.listdir(verification_folder):
            image_path = os.path.join(verification_folder, filename)
            verification_image = face_recognition.load_image_file(image_path)
            verification_encodings = face_recognition.face_encodings(verification_image)

            if len(verification_encodings) > 0:
                verification_encoding = verification_encodings[0]
                distance = face_recognition.face_distance([input_encoding], verification_encoding)[0]
                match = distance < threshold

                results.append({
                    'filename': filename,
                    'match': str(match),  # Convert boolean to string
                    'distance': distance
                })

                if match:
                    detection += 1
                    logging.info(f"{filename}: Match found with score {distance:.2f}")
                else:
                    logging.info(f"{filename}: No match, score {distance:.2f}")
            else:
                logging.warning(f"{filename}: No faces found in the image")

        # Update facial_result based on detection
        facial_result = "1" if detection > 0 else "0"
        logging.info(f"Updated facial_result: {facial_result}")

        highest_match = min([result['distance'] for result in results if result['match'] == "True"], default=None)
        verified = "true" if detection > 0 else "false"  # Convert boolean to string

        logging.info("Returning prediction results.")
        return jsonify({
            'results': results,
            'highest_match': highest_match,
            'detection': detection,
            'verified': verified,  # Use string representation
            'facial_result': facial_result
        })
    
    except Exception as e:
        logging.error(f"Error in upload_image function: {e}")
        return jsonify({'error': 'Internal server error'}), 500

if __name__ == '__main__':
    app.run(host='0.0.0.0', port=8081)
