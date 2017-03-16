#!/usr/bin/python


# Unintuitive steps:
#  Install the following:
#    sudo pip install -U setuptools
#    sudo pip install --upgrade google-api-python-client
#  Get an oauth key [I SHOULD HAVE TAKEN BETTER NOTES!!!]
#    Maybe this: https://console.developers.google.com/apis/credentials?project=youtubeicomments-157805
#  Use following client_secrets.json
#
#    {
#      "installed": {
#        "client_id": "***************",
#        "client_secret":"*************",
#        "redirect_uris": ["http://localhost", "urn:ietf:wg:oauth:2.0:oob"],
#        "auth_uri": "https://accounts.google.com/o/oauth2/auth",
#        "token_uri": "https://accounts.google.com/o/oauth2/token"
#      }
#    }
#
#  Use GUI here: https://developers.google.com/youtube/v3/live/docs/liveBroadcasts/list  (For testing)
#    Specify "snippet" as the "part"
#    Specify  "*********" as the "id" since listing doesn't seem to work.
#      "*********" will come from your broadcast ID available on stream page, after a few minutes of starting (I HOPE) seems flakey.
#
#    WHY CANT I GET CHAT ID FROM THIS API???
#  

import httplib2
import os
import sys

from apiclient.discovery import build
from apiclient.errors import HttpError
from oauth2client.client import flow_from_clientsecrets
from oauth2client.file import Storage
from oauth2client.tools import argparser, run_flow


# The CLIENT_SECRETS_FILE variable specifies the name of a file that contains
# the OAuth 2.0 information for this application, including its client_id and
# client_secret. You can acquire an OAuth 2.0 client ID and client secret from
# the {{ Google Cloud Console }} at
# {{ https://cloud.google.com/console }}.
# Please ensure that you have enabled the YouTube Data API for your project.
# For more information about using OAuth2 to access the YouTube Data API, see:
#   https://developers.google.com/youtube/v3/guides/authentication
# For more information about the client_secrets.json file format, see:
#   https://developers.google.com/api-client-library/python/guide/aaa_client_secrets
CLIENT_SECRETS_FILE = "client_secrets.json"

# This OAuth 2.0 access scope allows for read-only access to the authenticated
# user's account, but not other types of account access.
YOUTUBE_READONLY_SCOPE = "https://www.googleapis.com/auth/youtube.readonly"
YOUTUBE_READWRITE_SCOPE = "https://www.googleapis.com/auth/youtube"
YOUTUBE_API_SERVICE_NAME = "youtube"
YOUTUBE_API_VERSION = "v3"

# This variable defines a message to display if the CLIENT_SECRETS_FILE is
# missing.
MISSING_CLIENT_SECRETS_MESSAGE = """
WARNING: Please configure OAuth 2.0

To make this sample run you will need to populate the client_secrets.json file
found at:

   %s

with information from the {{ Cloud Console }}
{{ https://cloud.google.com/console }}

For more information about the client_secrets.json file format, please visit:
https://developers.google.com/api-client-library/python/guide/aaa_client_secrets
""" % os.path.abspath(os.path.join(os.path.dirname(__file__),
                                   CLIENT_SECRETS_FILE))

def get_authenticated_service(args):
  flow = flow_from_clientsecrets(CLIENT_SECRETS_FILE,
    scope=YOUTUBE_READONLY_SCOPE + " " + YOUTUBE_READWRITE_SCOPE + " " + "https://www.googleapis.com/auth/youtube.force-ssl",
    message=MISSING_CLIENT_SECRETS_MESSAGE)

  storage = Storage("%s-oauth2.json" % sys.argv[0])
  credentials = storage.get()

  if credentials is None or credentials.invalid:
    credentials = run_flow(flow, storage, args)

  return build(YOUTUBE_API_SERVICE_NAME, YOUTUBE_API_VERSION,
    http=credentials.authorize(httplib2.Http()))

# Retrieve a list of the liveStream resources associated with the currently
# authenticated user's channel.
def list_streams(youtube):
  print "Live streams:"

  list_streams_request = youtube.liveStreams().list(
    part="snippet", # << Snippet is critical
    id=" [this is your livestream idXXXXXXXXXXXx] ",
    maxResults=50
  )
  ## Consider using id = ....

  while list_streams_request:
    list_streams_response = list_streams_request.execute()
      #XXX WHY DOESNT THIS PART WORK?  Items is always empty.
    print list_streams_response
    print "%s" % list_streams_response
    for stream in list_streams_response.get("items", []):
      print "%s (%s)" % (stream["snippet"]["title"], stream["id"])

    list_streams_request = youtube.liveStreams().list_next(
      list_streams_request, list_streams_response)

    print "Done live Streams"

def list_messages(youtube):

  list_messages_request = youtube.liveChatMessages().list(
    liveChatId="xXXXXXXXXXXXXXXXXXXXXXXXXXXX",
    part="snippet",
    maxResults=200
  )

  while list_messages_request:
    list_streams_response = list_messages_request.execute()
    #resp = list_streams_response.get("", []);
    #print( dir( resp ) );
    message_list_response = list_streams_response.get("items", [])
    print message_list_response[-1]["snippet"]["displayMessage"];
    #for stream in message_list_response:
    #  print "%s " % (stream["snippet"]["displayMessage"] )
    #list_streams_request = youtube.liveStreams().list_next(
      #list_streams_request, list_streams_response)

if __name__ == "__main__":
  args = argparser.parse_args()

  youtube = get_authenticated_service(args)
  #print dir(youtube)

  try:
    list_streams(youtube)
    list_messages(youtube)
    #print "ok"
  except HttpError, e:
    print "An HTTP error %d occurred:\n%s" % (e.resp.status, e.content)

