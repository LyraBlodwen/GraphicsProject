import picIO
import numpy as np
import sys
import math
import random
from scipy.sparse import csr_matrix
from scipy.sparse.linalg import spsolve
from scipy.linalg import solve

window_size_half = 2
eps = 10

def colorize(file_name, file_type = 'bmp'):
	img, y, u, v = picIO.readFrom(file_name, file_type)
	img_m, y_m, u_m, v_m = picIO.readFrom(file_name+'_m', file_type)

	n, m, _ = np.shape(img)
	print("image size: ", n, 'x', m)

	data = []
	row = []
	col = []
	
	mark_white = np.zeros((n,m))

	for i in range(n):
		###################
		# show percentage #
		###################
		if i % 10 == 0:
			percentage = 100.0 * i / n
			sys.stdout.write("%.4f"%percentage);
			sys.stdout.write("%\r");
			sys.stdout.flush();
		for j in range(m):
			if y[i, j] > 0.95:
				mark_white[i, j] = True;
			######################
			# deal with diagonal #
			######################
			row.append(i*m+j)
			col.append(i*m+j)
			data.append(1.0)
			################################
			# speed up using interpolation #
			################################
			if y[i, j] != y_m[i, j] or i&1 or j&1:
				continue
			neighbors = []
			neighbors_val = []
			p_l = max(i - window_size_half, 0)
			p_r = min(i + window_size_half, n - 1) + 1
			q_l = max(j - window_size_half, 0)
			q_r = min(j + window_size_half, m - 1) + 1
			####################
			# append neighbors #
			####################
			for p in range(p_l, p_r, 2):
				for q in range(q_l, q_r, 2):
					if i != p or j != q:
						neighbors_val.append(y[p, q])
						neighbors.append((p, q))
			######################
			# calculate variance #
			######################
			sigma = np.var(neighbors_val) * 0.6
			mgv=np.min(np.square(neighbors_val-y[i, j]));
			if (sigma<(-mgv/math.log(0.01))):
				sigma=-mgv/math.log(0.01);
#			print(sigma)
			sigma = max(sigma, eps)
			sum = 0.0
			W = {}
			##################################
			# calculate W_{rs} and normalize #
			##################################
			for (p, q) in neighbors:
				W[p*m+q] = math.exp(- ((y[i, j] - y[p, q]) * (y[i, j] - y[p, q])) / sigma)
				sum += W[p*m+q]
#			print(sum)
			for (p, q) in neighbors:
				row.append(i*m+j)
				col.append(p*m+q)
				if sum < 0.01:
					data.append(-1)
				else:
					data.append(- W[p*m+q] / sum)

	######################
	# build sparse array #
	######################
	A = csr_matrix((data, (row, col)), (n*m, n*m))
	A = A.tocsr()
	print("matrix generated")

	b = u_m.flatten()
	u_r = spsolve(A, b)
	print(np.allclose(A.dot(u_r), b))

	b = v_m.flatten()
	v_r = spsolve(A, b)
	print(np.allclose(A.dot(v_r), b))

	picIO.printTo(file_name + '_out.' + file_type, y, u_r, v_r, n, m, mark_white)
	print("All done! Output image at " + file_name + '_out.' + file_type)

def velocity(x, y, Ix, Iy, It, n, m):
	A = []
	b = []
	for i in range(max(x-window_size_half-1, 0), min(x+window_size_half+1, n-1)):
		for j in range(max(y-window_size_half-1, 0), min(y+window_size_half+1, m-1)):
			A.append([Ix[i,j], Iy[i,j]])
			b.append([-It[i,j]])
	try:
		v = solve(np.transpose(A).dot(A), np.transpose(A).dot(b))
	except:
		return [0,0]
	return v

def colorize_video(file_name_pre, file_name, file_type = 'bmp'):
	img, y, u, v = picIO.readFrom(file_name, file_type)
	img_pre, y_pre, u_pre, v_pre = picIO.readFrom(file_name_pre, file_type)
	
	Ix, Iy = np.gradient(y)
	It = y - y_pre

	n, m, _ = np.shape(img)
	print("image size: ", n, 'x', m)

	data = []
	row = []
	col = []

	for i in range(n):
		###################
		# show percentage #
		###################
		if i % 10 == 0:
			percentage = 100.0 * i / n
			sys.stdout.write("%.4f"%percentage);
			sys.stdout.write("%\r");
			sys.stdout.flush();
		for j in range(m):
			######################
			# deal with diagonal #
			######################
			row.append(i*m+j)
			col.append(i*m+j)
			data.append(1.0)
			row.append(i*m+j+n*m)
			col.append(i*m+j+n*m)
			data.append(1.0)
			vc = velocity(i, j, Ix, Iy, It, n, m)
			################################
			# speed up using interpolation #
			################################
			if i&1 or j&1:
				continue
			neighbors = []
			neighbors_val = []
			p_l = max(i - window_size_half, 0)
			p_r = min(i + window_size_half, n - 1) + 1
			q_l = max(j - window_size_half, 0)
			q_r = min(j + window_size_half, m - 1) + 1
			####################
			# append neighbors #
			####################
			for p in range(p_l, p_r, 2):
				for q in range(q_l, q_r, 2):
					if i != p or j != q:
						neighbors_val.append(y[p, q])
						neighbors.append((p, q))
					if math.sqrt((i+vc[0]-p)**2 + (j+vc[1]-q)**2)<window_size_half:
						neighbors_val.append(y_pre[p, q])
						neighbors.append((p+n, q))
			######################
			# calculate variance #
			######################
			sigma = np.var(neighbors_val) * 0.6
			mgv=np.min(np.square(neighbors_val-y[i, j]));
			if (sigma<(-mgv/math.log(0.01))):
				sigma=-mgv/math.log(0.01);
#			print(sigma)
			sigma = max(sigma, eps)
			sum = 0.0
			W = {}
			##################################
			# calculate W_{rs} and normalize #
			##################################
			for (p, q) in neighbors:
				if p<n:
					W[p*m+q] = math.exp(- ((y[i, j] - y[p, q]) * (y[i, j] - y[p, q])) / sigma)
				else:
					W[p*m+q] = math.exp(- ((y[i, j] - y_pre[p-n, q]) * (y[i, j] - y_pre[p-n, q])) / sigma)
				sum += W[p*m+q]
#			print(sum)
			for (p, q) in neighbors:
				row.append(i*m+j)
				col.append(p*m+q)
				if sum < 0.01:
					data.append(-1)
				else:
					data.append(- W[p*m+q] / sum)

	######################
	# build sparse array #
	######################
	A = csr_matrix((data, (row, col)), (n*m*2, n*m*2))
	A = A.tocsr()
	print("matrix generated")

	b = np.concatenate((u.flatten(), u_pre.flatten()),axis=0)
	u_r = spsolve(A, b)
	print(np.allclose(A.dot(u_r), b))

	b = np.concatenate((v.flatten(), v_pre.flatten()),axis=0)
	v_r = spsolve(A, b)
	print(np.allclose(A.dot(v_r), b))

	picIO.printTo_(file_name + '_out.' + file_type, y, u_r[0:n*m], v_r[0:n*m], n, m)
	print("All done! Output image at " + file_name + '_out.' + file_type)


#img, y, u, v = picIO.readFrom('5', 'bmp')
#picIO.printGray('5_gray.bmp', y)
#picIO.printRandMarked('5_gray_m.bmp', img, 0.1, 5)
#colorize('5_gray')