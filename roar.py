import os
import logging
from slacker import Slacker

token = os.getenv('TREX')
print(token)
if token is None:
    logging.error("You'll need an API access token, and ensure it is set to the environment variable TREX")
    exit(1)

slack = Slacker(token=token)

# Send a message to #general channel
slack.chat.post_message('#random', 'ROAR!', as_user=True, link_names=1)