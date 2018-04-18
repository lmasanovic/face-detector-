import cv2
import time
import numpy as np
k = 1
EYE_CLOSED_FREAMES = 30
TIME_COUNTER = 0

povrsina = 0
povrsina_counter = 0
eyeClosedCounter = 0
head_counter_left = 0
head_counter_right = 0
head_counter_y = 0
starting_gray = 0


face_cascade = cv2.CascadeClassifier("/home/luka/opencv-3.1.0/data/haarcascades/haarcascade_frontalface_default.xml")
eye_cascade = cv2.CascadeClassifier("/home/luka/opencv-3.1.0/data/haarcascades/haarcascade_eye.xml")
#profileRight_cascade = cv2.CascadeClassifier("haarcascade_profileface_desno.xml")
profile_cascade = cv2.CascadeClassifier("17har.xml")
cap = cv2.VideoCapture("vid.mkv")

while (True):
	ret, img = cap.read()
	#img = cv2.flip(img, 1)
	gray = cv2.cvtColor(img, cv2.COLOR_BGR2GRAY)
	faces = face_cascade.detectMultiScale(gray, 1.2, 8)
	maxArea = 0
		
	if len(faces) > 0:
		head_counter_left = 0
		head_counter_right = 0		
		for (x, y, w, h) in faces:
			area = w*h
			if area > maxArea:
				maxArea = area
				face = (x, y, w, h)
		if maxArea > 0:
			k = 1
			x, y, w, h = face
			img = cv2.rectangle(img, (x, y), (x + w, y + h), (255, 0, 0), 2)
			roi_gray = gray[y:y + h, x:x + w]
			roi_color = img[y:y + h, x:x + w]
			eyes = eye_cascade.detectMultiScale(roi_gray, 1.3, 30)
			
			if (TIME_COUNTER == 0):
				#current_head_pose_x = x
				povrsina = w*h
				current_head_pose_y = y
				TIME_COUNTER += 1
				#print("Starting head pose x axis: {}".format(x))
				print("Starting head pose y axis: {}".format(y))
				print("Pocetna povrsina: {}".format(povrsina))
				time.sleep(2)
				
			for (ex, ey, ew, eh) in eyes:
				k = 0
				cv2.rectangle(roi_color, (ex, ey), (ex + ew, ey + eh), (255, 255, 0), 2)
		#print(w*h)
		updated_povrinsa = w*h
		if k == 1:
			eyeClosedCounter += 1
			if eyeClosedCounter >= EYE_CLOSED_FREAMES:
				cv2.putText(img, "WAKE UP!", (10, 85),
						cv2.FONT_HERSHEY_SIMPLEX, 0.7, (0, 0, 255), 2)
		else: 
			eyeClosedCounter = 0
			
		if updated_povrinsa > povrsina*1.3 or updated_povrinsa < povrsina*0.70: #bilo je 1.2 i 0.79 
			#print("priblizio se")
			povrsina_counter += 1
			if povrsina_counter > 10:
				if updated_povrinsa > povrsina*1.3 or updated_povrinsa < povrsina*0.70:
					povrsina = w*h
					current_head_pose_y = y
					print("Povrsina updatena: {}".format(povrsina))
					print("Head pose y updatena: {}".format(current_head_pose_y))
					#updated_povrinsa < povrsina*0.79: kad se udalji nazad .. updated_povrinsa > povrsina*1.2:  priblizi se
		else:
			if current_head_pose_y*1.1 < y:
					head_counter_y += 1
					if head_counter_y > 20:
						print("Head pose y: {}".format(current_head_pose_y))
						if current_head_pose_y*1.1 < y:
							cv2.putText(img, "HEAD DOWN!", (10, 30),
							cv2.FONT_HERSHEY_SIMPLEX, 0.7, (0, 0, 255), 2)
			else:
					head_counter_y = 0
				
	else:
		left = profile_cascade.detectMultiScale(gray, 1.1, 2)
		if len(left) > 0:
			head_counter_right = 0	
			head_counter_left += 1
			for (x, y, w, h) in left:
				cv2.rectangle(img, (x, y), (x + w, y + h), (255, 255, 0), 2)
			if head_counter_left > 20:
				cv2.putText(img, "TURN YOUR HEAD LEFT", (300, 30),
						cv2.FONT_HERSHEY_SIMPLEX, 0.7, (0, 0, 255), 2)
				
		else:
			head_counter_left = 0
			head_counter_right += 1
			img = cv2.flip(img, 1)
			gray = cv2.cvtColor(img, cv2.COLOR_BGR2GRAY)
			left = profile_cascade.detectMultiScale(gray, 1.1, 2)
			for (x, y, w, h) in left:
				cv2.rectangle(img, (x, y), (x + w, y + h), (255, 255, 0), 2)
			img = cv2.flip(img, 1)
			if head_counter_right > 20 and len(left) > 0:
				cv2.putText(img, "TURN YOUR HEAD RIGHT", (300, 30),
						cv2.FONT_HERSHEY_SIMPLEX, 0.7, (0, 0, 255), 2)

	cv2.namedWindow("img", cv2.WINDOW_NORMAL)
	cv2.resizeWindow("img", 600, 450)
	cv2.imshow('img', img)
	if cv2.waitKey(1) & 0xff == ord('q'):
		break
cap.release()
cv2.destroyAllWindows()

