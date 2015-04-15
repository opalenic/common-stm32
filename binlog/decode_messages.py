#!/usr/bin/python

'''
Created on 14. 4. 2015

@author: ondra
'''

import sys
import os
import ctypes

import json

data_type_size_lut = {
	'uint8_t': 1,
	'int8_t': 1,
	'uint16_t': 2,
	'int16_t': 2,
	'uint32_t': 4,
	'int32_t': 4,
	'uint64_t': 8,
	'int64_t': 8
}

conversion_lut = {
	'uint8_t': lambda buf: ctypes.c_uint8(buf[0]).value,
	'int8_t': lambda buf: ctypes.c_int8(buf[0]).value,
	'uint16_t': lambda buf: ctypes.c_uint16((buf[0] << 8) | buf[1]).value,
	'int16_t': lambda buf: ctypes.c_int16((buf[0] << 8) | buf[1]).value,
	'uint32_t': lambda buf: ctypes.c_uint32((buf[0] << 24) | (buf[1] << 16) | (buf[2] << 8) | buf[3]).value,
	'int32_t': lambda buf: ctypes.c_int32((buf[0] << 24) | (buf[1] << 16) | (buf[2] << 8) | buf[3]).value,
	'uint64_t': lambda buf: ctypes.c_uint64((buf[0] << 56) | (buf[1] << 48) | (buf[2] << 40) | (buf[3] << 32) | (buf[4] << 24) | (buf[5] << 16) | (buf[6] << 8) | buf[7]).value,
	'int64_t': lambda buf: ctypes.c_int64((buf[0] << 56) | (buf[1] << 48) | (buf[2] << 40) | (buf[3] << 32) | (buf[4] << 24) | (buf[5] << 16) | (buf[6] << 8) | buf[7]).value
}

my_dir = os.path.dirname(os.path.abspath(__file__))

with open(os.path.join(my_dir, sys.argv[1]), 'r') as config_file:
	messages = json.load(config_file)

	while True:		
		try:
			message_code = ord(sys.stdin.buffer.read(1))
			
			message_text = messages[message_code]['text']


			if 'args' in messages[message_code]:

				argument_buffer = sys.stdin.buffer.read(sum(map(lambda arg: data_type_size_lut[arg['type']], 				
															messages[message_code]['args'])))

				arguments = {}

				pos_in_buf = 0
				for arg in messages[message_code]['args']:
					arg_len = data_type_size_lut[arg['type']]
					arguments[arg['name']] = conversion_lut[arg['type']](argument_buffer[pos_in_buf:pos_in_buf + arg_len])

					pos_in_buf += arg_len


				message_text = message_text.format(**arguments)


			print(message_text)

		except IndexError:
			print("Unknown message code: {0}".format(message_code))
