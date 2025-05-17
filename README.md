# 🧑‍💻 Attendance Marking Using Facial Recognition (Image Verifier in C)

This project is a **C-based image verifier system** for marking attendance.  
It simulates face recognition by comparing a new image with pre-registered face data.  
Built entirely using **the C programming language**, this system is a prototype for attendance systems used in schools and colleges.



-------



## Objective

- Pre-register students by storing their face image (in good lighting conditions).
- Then, verify a student’s identity by comparing a one of the image with stored ones.
- If matched, mark the student's attendance in a `.csv` file.

---

## Folder Structure

```
Attendance-Marking-Using-Facial-Recognition/
├── src/
│   ├── main.c
│   ├── image_matcher.c
│   ├── attendance.c
│   └── ui.c
├── include/
│   ├── image_matcher.h
│   ├── attendance.h
│   └── ui.h
├── data/
│   ├── registered/
│   └── query/
├── logs/
│   └── attendance.csv
├── Makefile
└── README.md
```

