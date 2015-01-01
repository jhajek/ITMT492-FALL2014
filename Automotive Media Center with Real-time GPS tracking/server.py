""" 
 *  Description: This script checks for a connection and will print 
 *  to the console, when data is received. An SQL query will insert
 *  the data into the database.
 *  *
 *  This program is free software: you can redistribute it and/or modify 
 *  it under the terms of the GNU General Public License as published by 
 *  the Free Software Foundation, either version 3 of the License, or 
 *  (at your option) any later version. 
 *  
 *  This program is distributed in the hope that it will be useful, 
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of 
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the 
 *  GNU General Public License for more details.  
 *
 *  Version 1.0
 *  Author: Clint Bhola
 """
from twisted.internet.protocol import Factory, Protocol
from twisted.internet import reactor

import time
import _mysql
import config
import pprint

class SendContent(Protocol):
    def connectionMade(self):
        print 'Accepted connection'
    def dataReceived(self, data):
        date = time.time()
        values = data.split(',');
        if len(values) != 4:
            print 'Error: Bad Data'

        db = None;
        try:
            db = _mysql.connect(config.hostname, config.username, config.password, config.database)
            db.query("INSERT INTO tracker (date, speed, lat, lng, course)" +
                "VALUES ('" + str(date) + "', '" + str(values[0]) + "', '" + 
                str(values[1]) + "', '" + str(values[2]) + "', '" + str(values[3]) + "')")
            print "---------------------"
            print "| Data Logged       |"
            print "---------------------"
            print "Speed: " + values[0]
            print "Latitude: " + values[1]
            print "Longitude: " + values[2]
            print "Course: " + values[3]
        except _mysql.Error, e:
            print "Error %d: %s" % (e.args[0], e.args[1])
        finally:
            if db: 
                db.close()

class SendContentFactory(Factory):
    protocol = SendContent

reactor.listenTCP(81, SendContentFactory())
reactor.run()

