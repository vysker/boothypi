from subprocess import call
from time import sleep, gmtime, strftime
import RPi.GPIO as GPIO

# Destination path of the preview file.
PREVIEW_FILE_PATH = '/home/dick/dev/pi/boothypiui/feed/preview/preview.jpg'
# Destination directory for the camera roll.
ROLL_DIR = '/home/dick/dev/pi/boothypiui/feed/roll/'

DATETIME_FORMAT = '%Y-%m-%d_%H-%M-%S'

# Constant that holds the command for capturing preview images.
# This prevents constant reinitialization of the array in the while-loop.
PREVIEW_COMMAND_ARRAY = ['gphoto2', '--capture-image-and-download', '--filename=' + PREVIEW_FILE_PATH, '--force-overwrite']

# The amount of time to wait before the next picture is taken.
PREVIEW_INTERVAL = 0.250

# When set to true, this flag indicates that the camera is currently not available for use.
camera_busy = False

def take_picture(channel):
  camera_busy = True
  PATH = ROLL_DIR + 'roll' + strftime(DATETIME_FORMAT, gmtime())
  call([
    'gphoto2',
    '--capture-image-and-download',
    '--filename=' + PATH,
    '--force-overwrite'
  ])
  camera_busy = False
  # call(['cp', ]) # TODO: copy to external drive

GPIO.add_event_detect(channel, GPIO.RISING, callback=take_picture)

while True:
  if camera_busy == False:
    call(PREVIEW_COMMAND_ARRAY)
  
  sleep(PREVIEW_INTERVAL)
