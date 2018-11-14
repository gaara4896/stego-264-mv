import cv2

original_img = cv2.imread('original_footage.tif')
hidenseek_img = cv2.imread('hidenseek_footage.tif')
rand_hidenseek_img = cv2.imread('rand-hidenseek_footage.tif')
mvsteg_img = cv2.imread('mvsteg_footage.tif')

cv2.imwrite("original_hidenseek_subtract.tif", cv2.subtract(original_img, hidenseek_img))
cv2.imwrite("original_rand-hidenseek_subtract.tif", cv2.subtract(original_img, rand_hidenseek_img))
cv2.imwrite("original_mvsteg_subtract.tif", cv2.subtract(original_img, mvsteg_img))
cv2.imwrite("hidenseek_rand-hidenseek_subtract.tif", cv2.subtract(hidenseek_img, rand_hidenseek_img))
cv2.imwrite("hidenseek_mvsteg_subtract.tif", cv2.subtract(hidenseek_img, mvsteg_img))
cv2.imwrite("rand-hidenseek_mvsteg_subtract.tif", cv2.subtract(rand_hidenseek_img, mvsteg_img))

cv2.imwrite("original_hidenseek_divide.tif", cv2.divide(original_img, hidenseek_img))
cv2.imwrite("original_rand-hidenseek_divide.tif", cv2.divide(original_img, rand_hidenseek_img))
cv2.imwrite("original_mvsteg_divide.tif", cv2.divide(original_img, mvsteg_img))
cv2.imwrite("hidenseek_rand-hidenseek_divide.tif", cv2.divide(hidenseek_img, rand_hidenseek_img))
cv2.imwrite("hidenseek_mvsteg_divide.tif", cv2.divide(hidenseek_img, mvsteg_img))
cv2.imwrite("rand-hidenseek_mvsteg_divide.tif", cv2.divide(rand_hidenseek_img, mvsteg_img))
