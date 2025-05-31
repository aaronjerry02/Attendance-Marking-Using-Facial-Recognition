import cv2 #for computer vision tasks
import os #for files and directories
import numpy as np #for array ops in image processing

# Define directories
#python script directory
base_dir = os.path.dirname(os.path.abspath(__file__))
train_dir = os.path.join(base_dir, "train")
test_img_path = os.path.join(base_dir, "test", "test_image.png")

# Load face cascade classifier which analyse and locate image
face_cascade = cv2.CascadeClassifier(cv2.data.haarcascades + 'haarcascade_frontalface_default.xml')

faces = []
labels = []
label_map = {}
label_id = 0  

# iterate through train data
for person_name in os.listdir(train_dir):
    person_path = os.path.join(train_dir, person_name)
    if not os.path.isdir(person_path):
        continue

    label_map[label_id] = person_name

    for img_name in os.listdir(person_path):
        img_path = os.path.join(person_path, img_name)
        img = cv2.imread(img_path)
        if img is None:
            continue # Skip unreadable images

        gray = cv2.cvtColor(img, cv2.COLOR_BGR2GRAY)
        detected = face_cascade.detectMultiScale(gray, 1.1, 5)

        for (x, y, w, h) in detected:
            face = gray[y:y+h, x:x+w]
            face = cv2.resize(face, (200, 200))
            faces.append(face)
            labels.append(label_id)
            break # Process only the first detected face per image
    label_id += 1

# Train the recognizer
recognizer = cv2.face.LBPHFaceRecognizer_create()

if len(faces) > 0:
    recognizer.train(faces, np.array(labels))
else:
    with open("recognized_name.txt", "w") as f:
        f.write("UNKNOWN")
    exit()

# Load and process test image
test_img = cv2.imread(test_img_path)
if test_img is None:
    with open("recognized_name.txt", "w") as f:
        f.write("UNKNOWN")
    exit()

gray_test = cv2.cvtColor(test_img, cv2.COLOR_BGR2GRAY)
detected = face_cascade.detectMultiScale(gray_test, 1.1, 5)

predicted_name = "UNKNOWN"
CONFIDENCE_THRESHOLD = 50.0

# Perform recognition
for (x, y, w, h) in detected:
    face = gray_test[y:y+h, x:x+w]
    face = cv2.resize(face, (200, 200))

    label, confidence = recognizer.predict(face)

    if confidence < CONFIDENCE_THRESHOLD:
        predicted_name = label_map[label]
    else:
        predicted_name = "UNKNOWN"
    break # Process only the first detected face in test image
else:
    predicted_name = "UNKNOWN"

# Save result for C program
with open("recognized_name.txt", "w") as f:
    f.write(predicted_name)

# Only print the final recognition result
print(f"Recognized name: '{predicted_name}'")
