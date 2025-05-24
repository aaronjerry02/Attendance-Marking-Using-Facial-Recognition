import cv2
import os
import numpy as np

# Base path relative to this script's location
base_dir = os.path.dirname(os.path.abspath(__file__))
train_dir = os.path.join(base_dir, "train")
test_img_path = os.path.join(base_dir, "test", "test_image.png")

# Load Haar cascade
face_cascade = cv2.CascadeClassifier(cv2.data.haarcascades + 'haarcascade_frontalface_default.xml')

# Data for training
faces = []
labels = []
label_map = {}
label_id = 0

# Load and label training images
for person_name in os.listdir(train_dir):
    person_path = os.path.join(train_dir, person_name)
    if not os.path.isdir(person_path):
        continue

    label_map[label_id] = person_name

    for img_name in os.listdir(person_path):
        img_path = os.path.join(person_path, img_name)
        img = cv2.imread(img_path)
        gray = cv2.cvtColor(img, cv2.COLOR_BGR2GRAY)

        detected = face_cascade.detectMultiScale(gray, scaleFactor=1.1, minNeighbors=5)
        for (x, y, w, h) in detected:
            face = gray[y:y+h, x:x+w]
            face = cv2.resize(face, (200, 200))
            faces.append(face)
            labels.append(label_id)
            break

    label_id += 1

# Train the face recognizer
recognizer = cv2.face.LBPHFaceRecognizer_create()
recognizer.train(faces, np.array(labels))

# Load test image
test_img = cv2.imread(test_img_path)
gray_test = cv2.cvtColor(test_img, cv2.COLOR_BGR2GRAY)
detected = face_cascade.detectMultiScale(gray_test, 1.1, 5)

for (x, y, w, h) in detected:
    face = gray_test[y:y+h, x:x+w]
    face = cv2.resize(face, (200, 200))
    label, confidence = recognizer.predict(face)

    name = label_map[label]
    print(f"Prediction: {name} (Confidence: {confidence:.2f})")

    cv2.rectangle(test_img, (x, y), (x+w, y+h), (0, 255, 0), 2)
    cv2.putText(test_img, f"{name} ({confidence:.0f})", (x, y - 10),
                cv2.FONT_HERSHEY_SIMPLEX, 0.9, (0, 255, 0), 2)

# Resize image to 50% of its original size for display
resized_img = cv2.resize(test_img, None, fx=0.5, fy=0.5)  # fx and fy are scale factors

cv2.imshow("Prediction", resized_img)
cv2.waitKey(0)
cv2.destroyAllWindows()
