#!/usr/bin/python

'''
Created on 7. 4. 2015

@author: ondra
'''

import os
import sys
import json

import pystache
import datetime


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

arg_conversion_lut = {
	'uint8_t': [
		"{arg_name}"
	],
	'int8_t': [
		"(uint8_t) {arg_name}"
	],
	'uint16_t': [
		"(uint8_t) ({arg_name} >> 8)",
		"(uint8_t) {arg_name}"
	],
	'int16_t': [
		"(uint8_t) (((uint16_t) {arg_name}) >> 8)",
		"(uint8_t) {arg_name}"
	],
	'uint32_t': [
		"(uint8_t) ({arg_name} >> 24)",
		"(uint8_t) ({arg_name} >> 16)",
		"(uint8_t) ({arg_name} >> 8)",
		"(uint8_t) {arg_name}"
	],
	'int32_t': [
		"(uint8_t) (((uint32_t) {arg_name}) >> 24)",
		"(uint8_t) (((uint32_t) {arg_name}) >> 16)",
		"(uint8_t) (((uint32_t) {arg_name}) >> 8)",
		"(uint8_t) {arg_name}"
	],
	'uint64_t': [
		"(uint8_t) ({arg_name} >> 56)",
		"(uint8_t) ({arg_name} >> 48)",
		"(uint8_t) ({arg_name} >> 40)",
		"(uint8_t) ({arg_name} >> 32)",
		"(uint8_t) ({arg_name} >> 24)",
		"(uint8_t) ({arg_name} >> 16)",
		"(uint8_t) ({arg_name} >> 8)",
		"(uint8_t) {arg_name}"
	],
	'int64_t': [
		"(uint8_t) (((uint64_t) {arg_name}) >> 56)",
		"(uint8_t) (((uint64_t) {arg_name}) >> 48)",
		"(uint8_t) (((uint64_t) {arg_name}) >> 40)",
		"(uint8_t) (((uint64_t) {arg_name}) >> 32)",
		"(uint8_t) (((uint64_t) {arg_name}) >> 24)",
		"(uint8_t) (((uint64_t) {arg_name}) >> 16)",
		"(uint8_t) (((uint64_t) {arg_name}) >> 8)",
		"(uint8_t) {arg_name}"
	]
}

my_dir = os.path.dirname(os.path.abspath(__file__))

with open(os.path.join(my_dir, sys.argv[1]), 'r') as config_file:
	config = json.load(config_file)
	
	model = {
		'log_msg_types': [],
		'generated_on': datetime.datetime.now().strftime('%c'),
		'func_prefix_uppercase': sys.argv[2].upper(),
		'func_prefix_lowercase': sys.argv[2].lower(),
	}

	msg_code = 0

	for msg in config:

		msg_model = {
			'name_uppercase': msg['name'].upper(),
			'name_lowercase': msg['name'].lower(),
			'code': msg_code,
			'text': msg['text']
		}

		msg_code += 1

		if 'args' in msg:
			msg_model['has_arguments'] = True

			msg_model['function_param_str'] = ", ".join(
				map(lambda arg: "{0} {1}".format(arg['type'], arg['name']), msg['args']))

			msg_model['argument_buffer_len'] = sum(map(lambda arg: data_type_size_lut[arg['type']], msg['args']))

			msg_model['argument_to_bytes'] = []

			pos_in_buf = 0
			for arg in msg['args']:

				conversion_list = []
				for i, conversion in enumerate(arg_conversion_lut[arg['type']]):
					conversion_list.append({
						'pos': pos_in_buf + i, 
						'conversion': conversion.format(arg_name=arg['name'])
					})

				msg_model['argument_to_bytes'].append(conversion_list)
				pos_in_buf += data_type_size_lut[arg['type']]

		else:
			msg_model['has_arguments'] = False
			msg_model['function_param_str'] = "void"

		model['log_msg_types'].append(msg_model)


	with open(os.path.join(my_dir, 'binlog.c.mustache'), 'r') as c_template_file:

		with open(os.path.join(my_dir, '{}.c'.format(sys.argv[2].lower())), 'w') as c_output_file:

			c_output_file.write(pystache.render(c_template_file.read(), model))


	with open(os.path.join(my_dir, 'binlog.h.mustache'), 'r') as h_template_file:

		with open(os.path.join(my_dir, '{}.h'.format(sys.argv[2].lower())), 'w') as h_output_file:

			h_output_file.write(pystache.render(h_template_file.read(), model))
