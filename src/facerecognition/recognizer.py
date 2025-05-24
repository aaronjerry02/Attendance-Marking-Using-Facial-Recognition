import cv2
import os
import numpy as np

base_dir = os.path.dirname(os.path.abspath(__file__))
train_dir = os.path.join(base_dir, "train")
test_img_path = os.path.join(base_dir, "test", "test_image.png")

face_cascade = cv2.CascadeClassifier(cv2.data.haarcascades + 'haarcascade_frontalface_default.xml')

faces = []
labels = []
label_map = {}
label_id = 0

for person_name in os.listdir(train_dir):
    person_path = os.path.join(train_dir, person_name)
    if not os.path.isdir(person_path):
        continue

    label_map[label_id] = person_name

    for img_name in os.listdir(person_path):
        img_path = os.path.join(person_path, img_name)
        img = cv2.imread(img_path)
        gray = cv2.cvtColor(img, cv2.COLOR_BGR2GRAY)
        detected = face_cascade.detectMultiScale(gray, 1.1, 5)

        for (x, y, w, h) in detected:
            face = gray[y:y+h, x:x+w]
            face = cv2.resize(face, (200, 200))
            faces.append(face)
            labels.append(label_id)
            break

    label_id += 1

recognizer = cv2.face.LBPHFaceRecognizer_create()
recognizer.train(faces, np.array(labels))

test_img = cv2.imread(test_img_path)
gray_test = cv2.cvtColor(test_img, cv2.COLOR_BGR2GRAY)
detected = face_cascade.detectMultiScale(gray_test, 1.1, 5)

predicted_name = "Unknown"

for (x, y, w, h) in detected:
    face = gray_test[y:y+h, x:x+w]
    face = cv2.resize(face, (200, 200))
    label, confidence = recognizer.predict(face)
    predicted_name = label_map[label]
    break


# Save to file for C to read
with open("recognized_name.txt", "w") as f:
    f.write(predicted_name)
