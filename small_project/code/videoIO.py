import os
import picIO
import colorize

total_frames = 100
pic_type = 'bmp'
print_gray = False
print_marked = True
every_marked = 20

#os.system('ffmpeg -r 30 -i video_frames/out-%03d.bmp -c:v libx264 -vf fps=25 -pix_fmt yuv420p out.mp4')

#ffmpeg -i lzz.mp4 video_frames/out-%03d.bmp

last_marked = 0
if (False):
	for i in range(1, total_frames + 1):
		img, y, u, v = picIO.readFrom('video_frames/out-%03d'%(i), pic_type)
		if print_gray:
			picIO.printGray('video_frames/img-%03d.'%(i) + pic_type, y)
		elif print_marked and i % every_marked == 1:
			picIO.printRandMarked('video_frames/img-%03d_m.'%(i) + pic_type, img, 0.1, 5)
	#	elif i % every_marked != 1:
	#		picIO.transferMark('video_frames/img-%03d'%(last_marked), 'video_frames/img-%03d'%(i), pic_type)
		if i % every_marked == 1:
			last_marked = i

for i in range(1, total_frames + 1):
	if i % every_marked == 1:
			colorize.colorize('video_frames/img-%03d'%(i))
	else:
		colorize.colorize_video('video_frames/img-%03d_out'%(i-1), 'video_frames/img-%03d'%(i))