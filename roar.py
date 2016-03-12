import os
import logging
from datetime import datetime
from slacker import Slacker
from flask import Flask

app = Flask(__name__)

slack = None

g = {}

MIN_ROAR_INTERVAL = 5  # Do not roar if we roared within X seconds

@app.route('/roar')
def hello_world():
    global slack, g
    if not slack:
        setup()
    # Send a message to #random channel
    now = datetime.now()

    is_set = False
    if "last_roared" not in g:
        g['last_roared'] = now
        is_set = True

    interval = (now - g['last_roared']).seconds
    if  interval >= MIN_ROAR_INTERVAL:
        slack.chat.post_message('#random', 'ROAR!', as_user=True, link_names=1)
        g['last_roared'] = now
        is_set = True
        return "Roared: {} ({}s since) is_set={}".format(g['last_roared'], interval, is_set)
    else:
        return "Too quick: {} ({}s since) is_set={}".format(g['last_roared'], interval, is_set)


def setup():
    global slack
    token = os.getenv('TREX')
    if token is None:
        logging.error("You'll need an API access token, and ensure it is set to the environment variable TREX")
        exit(1)
    slack = Slacker(token=token)


if __name__ == "__main__":
    setup()
    app.run(host='0.0.0.0', debug=True, port=5002)