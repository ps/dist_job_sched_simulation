import Image
import ImageFont, ImageDraw
import sys

test_case_num = sys.argv[1]
node1 = sys.argv[2]
node2 = sys.argv[3]

#opens an image:
test_suite = "varied"
test_folder = "results_%s_1000" % test_suite
test_case = "%s_%s" % (test_suite, test_case_num)
test_case_text = "Test: %s %s" % (test_suite.upper(), test_case_num)
out_file = "case_%s_%s.png" % (test_suite, test_case_num)

im1 = Image.open("%s/%s/thread-1-assign_rate.png" % (test_folder, test_case))
im2 = Image.open("%s/%s/thread-1-jobs_remaining.png" % (test_folder, test_case))
im3 = Image.open("%s/%s/thread%s-queue_size.png" % (test_folder, test_case, node1))
im4 = Image.open("%s/%s/thread%s-queue_size.png" % (test_folder, test_case, node2))
#creates a new empty image, RGB mode, and size 400 by 400.
height_offset=40
new_im = Image.new('RGB', (1600, 1200 + height_offset), color="white")

new_im.paste(im1, (0, 0 + height_offset))
new_im.paste(im2, (800, 0 + height_offset))
new_im.paste(im3, (0, 600 + height_offset))
new_im.paste(im4, (800, 600 + height_offset))


draw = ImageDraw.Draw(new_im)
font = ImageFont.truetype("LiberationSans-Regular.ttf", 40)
draw.text((650, 10), test_case_text, font=font, fill="red")

new_im.thumbnail((400, 300))

new_im.save(out_file)
