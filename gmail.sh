#!/bin/bash

curl -u 'zezeon1:Qwer!234' --silent "https://mail.google.com/mail/feed/atom" |
tr -d '\n' |
sed 's:</entry>:\n:g' |
sed -n 's/.*<title>\(.*\)<\/title.*<author><name>\([^<]*\)<\/name><email>\([^<]*\).*/From: \2 [\3] \nSubject: \1\n/p'
