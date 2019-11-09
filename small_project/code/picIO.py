import imageio
import numpy as np
import random

def rgb2yuv(rgb):
	rgb = rgb / 255.0
	yuv = np.zeros_like(rgb)
	yuv[:, :, 0] = np.clip(np.dot(rgb, np.array([0.299, 0.587, 0.144])),             0,   1)
	yuv[:, :, 1] = np.clip(np.dot(rgb, np.array([0.595716, -0.274453, -0.321263])), -0.5957, 0.5957)
	yuv[:, :, 2] = np.clip(np.dot(rgb, np.array([0.211456, -0.522591, 0.311135])),  -0.5226, 0.5226)
	return yuv

def yuv2rgb(yuv):
	rgb = np.zeros_like(yuv)
	rgb[:, :, 0] = np.dot(yuv, np.array([1.0,  0.956295719758948,  0.621024416465261]))
	rgb[:, :, 1] = np.dot(yuv, np.array([1.0, -0.272122099318510, -0.647380596825695]))
	rgb[:, :, 2] = np.dot(yuv, np.array([1.0, -1.106989016736491,  1.704614998364648]))
	return np.clip(rgb * 255.0, 0.0, 255.0)

def readFrom(picName, picType):
	'''
		read from file
		convert to yuv
		divide 255.0
		return tuple (img, y, u, v)
	'''
	img = imageio.imread(picName + '.' + picType)
	img = rgb2yuv(img)
	y = img[:, :, 0]
	u = img[:, :, 1]
	v = img[:, :, 2]
	print('read ' + picName + '.' + picType)
	return (img, y, u, v)

def printGray(path, y):
	'''
		convert to rgb with only y (luminance)
		print the gray pic to file
	'''
	img = np.zeros((y.shape[0], y.shape[1], 3))
	img[:, :, 0] = y[:, :]
	img = yuv2rgb(img)
	imageio.imsave(path, img.astype(np.uint8))

def printRandMarked(path, img, rate, blocksize = 30):
	'''
		reserve rate percentage color information
		convert to rgb from yuv
		print to file
	'''
	marked = np.zeros_like(img)
	marked[:, :, 0] = img[:, :, 0]
	for i in range(0, img.shape[0], blocksize):
		for j in range(0, img.shape[1], blocksize):
			if random.random() < rate:
				for p in range(i, min(i+blocksize, img.shape[0])):
					for q in range(j, min(j+blocksize, img.shape[1])):
						marked[p, q, 1] = img[p, q, 1]
						marked[p, q, 2] = img[p, q, 2]
	marked = yuv2rgb(marked)
	imageio.imsave(path, marked.astype(np.uint8))

def printTo_(path, y, u, v, n, m):
	'''
		print to file using y, u, v
		dimension nxm
		recover missing points by interpolation (to speed up the calculation)
	'''
	img = np.zeros((n, m, 3))
	img[:, :, 0] = y[:, :]
	img[:, :, 1] = u.reshape((n, m))
	img[:, :, 2] = v.reshape((n, m))
	for i in range(n):
			for j in range(m):
				if i&1 or j&1:
					s1 = s2 = ts = 0.0
					for p in range(max(i-1, 0), min(i+1, n-1) + 1):
						for q in range(max(j-1, 0), min(j+1, m-1) + 1):
							if not (p&1 or q&1):
								ts += 1
								s1 += img[p, q, 1]
								s2 += img[p, q, 2]
					img[i, j ,1] = s1 / ts
					img[i, j, 2] = s2 / ts
	img = yuv2rgb(img)
	imageio.imsave(path, img.astype(np.uint8))

def printTo(path, y, u, v, n, m, mark_white):
	'''
		print to file using y, u, v
		dimension nxm
		recover missing points by interpolation (to speed up the calculation)
	'''
	img = np.zeros((n, m, 3))
	img[:, :, 0] = y[:, :]
	img[:, :, 1] = u.reshape((n, m))
	img[:, :, 2] = v.reshape((n, m))
	for i in range(n):
		for j in range(m):
			if mark_white[i, j]:
#				img[i, j, 0] = 0.95
				img[i, j, 1] = img[i, j, 2] =  0
	for i in range(n):
		for j in range(m):
			if i&1 or j&1:
				s1 = s2 = ts = 0.0
				for p in range(max(i-1, 0), min(i+1, n-1) + 1):
					for q in range(max(j-1, 0), min(j+1, m-1) + 1):
						if not (p&1 or q&1):
							ts += 1
							s1 += img[p, q, 1]
							s2 += img[p, q, 2]
				img[i, j ,1] = s1 / ts
				img[i, j, 2] = s2 / ts
	img = yuv2rgb(img)
	imageio.imsave(path, img.astype(np.uint8))

def transferMark(picFrom, picTo, picType):
	img = imageio.imread(picFrom + '.' + picType)
	img_m = imageio.imread(picFrom + '_m.' + picType)
	img_to = imageio.imread(picTo + '.' + picType)
	if img.shape != img_to.shape:
		print("different dimension")
		return
	for i in range(img.shape[0]):
		for j in range(img.shape[1]):
			if img[i, j, 0] != img_m[i, j, 0] or img[i, j, 1] != img_m[i, j, 1] or img[i, j, 2] != img_m[i, j, 2]:
				img_to[i, j, 0] = img_m[i, j, 0]
				img_to[i, j, 1] = img_m[i, j, 1]
				img_to[i, j, 2] = img_m[i, j, 2]
	imageio.imsave(picTo + '_m.' + picType, img_to.astype(np.uint8))