from subprocess import call
from time import sleep, gmtime, strftime
import os
import errno
import RPi.GPIO as GPIO

# Destination dir of the preview file.
PREVIEW_FILE_DIR = '/home/pi/dev/boothypi-ui/feed/preview/'
# Filename of the preview file.
PREVIEW_FILE_NAME = 'preview.jpg'
# Full path of the preview file.
PREVIEW_FILE_PATH = PREVIEW_FILE_DIR + '' + PREVIEW_FILE_NAME
# Destination directory for the camera roll.
ROLL_DIR = '/home/pi/dev/boothypi-ui/feed/roll/'

def try_create_dir(path):
  print('Creating ' + path)
  try:
    os.makedirs(path)
    print('Successfully created ' + path)
  except OSError as e:
    if e.errno == errno.EEXIST:
      print('Did not create dir because it already exists (' + path + ').')

print('Creating required dirs')
try_create_dir(PREVIEW_FILE_DIR)
try_create_dir(ROLL_DIR)
print('Required dirs created')

PIN_SNAP_BUTTON = 23

GPIO.setmode(GPIO.BCM)
GPIO.setup(PIN_SNAP_BUTTON, GPIO.IN)

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

GPIO.add_event_detect(PIN_SNAP_BUTTON, GPIO.RISING, callback=take_picture)

while True:
  if camera_busy == False:
    call(PREVIEW_COMMAND_ARRAY)
  
  sleep(PREVIEW_INTERVAL)
