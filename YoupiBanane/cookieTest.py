#!/usr/bin/python

import os
from http.cookies import SimpleCookie

cookie_header = os.environ.get('HTTP_COOKIE')

if cookie_header:
    cookie = SimpleCookie()
    cookie.load(cookie_header)

    if 'id' in cookie:
        print("Value of id cookie:", cookie['id'].value)
    else:
        print("Cookie 'id' not found in the cookie header.")
else:
    print("No cookie header found in environment variables.")
