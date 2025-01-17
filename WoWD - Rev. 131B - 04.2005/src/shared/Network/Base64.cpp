/**
 **	File ......... Base64.cpp
 **	Published ....  2004-02-13
 **	Author ....... grymse@alhem.net
**/
/*
Copyright (C) 2004  Anders Hedstrom

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
*/
#include "Base64.h"


char *Base64::bstr =
	"ABCDEFGHIJKLMNOPQ"
	"RSTUVWXYZabcdefgh"
	"ijklmnopqrstuvwxy"
	"z0123456789+/";

char Base64::rstr[] = {
	  0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0, 
	  0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0, 
	  0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,  62,   0,   0,   0,  63, 
	 52,  53,  54,  55,  56,  57,  58,  59,  60,  61,   0,   0,   0,   0,   0,   0, 
	  0,   0,   1,   2,   3,   4,   5,   6,   7,   8,   9,  10,  11,  12,  13,  14, 
	 15,  16,  17,  18,  19,  20,  21,  22,  23,  24,  25,   0,   0,   0,   0,   0, 
	  0,  26,  27,  28,  29,  30,  31,  32,  33,  34,  35,  36,  37,  38,  39,  40, 
	 41,  42,  43,  44,  45,  46,  47,  48,  49,  50,  51,   0,   0,   0,   0,   0};


Base64::Base64()
{
//	for (int i = 0; i < 64; i++)
//		rstr[(int)bstr[i]] = i;
}


void Base64::encode(FILE *fil, std::string& output, bool add_crlf)
{
	size_t remain;
	size_t i = 0;
	size_t o = 0;
	char input[4];

	output = "";
	remain = fread(input,1,3,fil);
	while (remain > 0)
	{
		if (add_crlf && o && o % 76 == 0)
			output += "\n";
		switch (remain)
		{
		case 1:
			output += bstr[ ((input[i] >> 2) & 0x3f) ];
			output += bstr[ ((input[i] << 4) & 0x30) ];
			output += "==";
			break;
		case 2:
			output += bstr[ ((input[i] >> 2) & 0x3f) ];
			output += bstr[ ((input[i] << 4) & 0x30) + ((input[i + 1] >> 4) & 0x0f) ];
			output += bstr[ ((input[i + 1] << 2) & 0x3c) ];
			output += "=";
			break;
		default:
			output += bstr[ ((input[i] >> 2) & 0x3f) ];
			output += bstr[ ((input[i] << 4) & 0x30) + ((input[i + 1] >> 4) & 0x0f) ];
			output += bstr[ ((input[i + 1] << 2) & 0x3c) + ((input[i + 2] >> 6) & 0x03) ];
			output += bstr[ (input[i + 2] & 0x3f) ];
		}
		o += 4;
		//
		remain = fread(input,1,3,fil);
	}
}


void Base64::encode(const std::string& str_in, std::string& str_out, bool add_crlf)
{
	encode(str_in.c_str(), str_in.size(), str_out, add_crlf);
}


void Base64::encode(const char* input,size_t l,std::string& output, bool add_crlf)
{
	size_t i = 0;
	size_t o = 0;
	
	output = "";
	while (i < l)
	{
		size_t remain = l - i;
		if (add_crlf && o && o % 76 == 0)
			output += "\n";
		switch (remain)
		{
		case 1:
			output += bstr[ ((input[i] >> 2) & 0x3f) ];
			output += bstr[ ((input[i] << 4) & 0x30) ];
			output += "==";
			break;
		case 2:
			output += bstr[ ((input[i] >> 2) & 0x3f) ];
			output += bstr[ ((input[i] << 4) & 0x30) + ((input[i + 1] >> 4) & 0x0f) ];
			output += bstr[ ((input[i + 1] << 2) & 0x3c) ];
			output += "=";
			break;
		default:
			output += bstr[ ((input[i] >> 2) & 0x3f) ];
			output += bstr[ ((input[i] << 4) & 0x30) + ((input[i + 1] >> 4) & 0x0f) ];
			output += bstr[ ((input[i + 1] << 2) & 0x3c) + ((input[i + 2] >> 6) & 0x03) ];
			output += bstr[ (input[i + 2] & 0x3f) ];
		}
		o += 4;
		i += 3;
	}
}


void Base64::encode(unsigned char* input,size_t l,std::string& output,bool add_crlf)
{
	size_t i = 0;
	size_t o = 0;
	
	output = "";
	while (i < l)
	{
		size_t remain = l - i;
		if (add_crlf && o && o % 76 == 0)
			output += "\n";
		switch (remain)
		{
		case 1:
			output += bstr[ ((input[i] >> 2) & 0x3f) ];
			output += bstr[ ((input[i] << 4) & 0x30) ];
			output += "==";
			break;
		case 2:
			output += bstr[ ((input[i] >> 2) & 0x3f) ];
			output += bstr[ ((input[i] << 4) & 0x30) + ((input[i + 1] >> 4) & 0x0f) ];
			output += bstr[ ((input[i + 1] << 2) & 0x3c) ];
			output += "=";
			break;
		default:
			output += bstr[ ((input[i] >> 2) & 0x3f) ];
			output += bstr[ ((input[i] << 4) & 0x30) + ((input[i + 1] >> 4) & 0x0f) ];
			output += bstr[ ((input[i + 1] << 2) & 0x3c) + ((input[i + 2] >> 6) & 0x03) ];
			output += bstr[ (input[i + 2] & 0x3f) ];
		}
		o += 4;
		i += 3;
	}
}


void Base64::decode(const std::string& input,std::string& output)
{
	size_t i = 0;
	size_t l = input.size();
	
	output = "";
	while (i < l)
	{
		while (i < l && (input[i] == 13 || input[i] == 10))
			i++;
		if (i < l)
		{
			char b1 = (rstr[(int)input[i]] << 2 & 0xfc) +
					(rstr[(int)input[i + 1]] >> 4 & 0x03);
			output += b1;
			if (input[i + 2] != '=')
			{
				char b2 = (rstr[(int)input[i + 1]] << 4 & 0xf0) +
						(rstr[(int)input[i + 2]] >> 2 & 0x0f);
				output += b2;
			}
			if (input[i + 3] != '=')
			{
				char b3 = (rstr[(int)input[i + 2]] << 6 & 0xc0) +
						rstr[(int)input[i + 3]];
				output += b3;
			}
			i += 4;
		}
	}
}


void Base64::decode(const std::string& input, unsigned char *output, size_t& sz)
{
	size_t i = 0;
	size_t l = input.size();
	size_t j = 0;
	
	while (i < l)
	{
		while (i < l && (input[i] == 13 || input[i] == 10))
			i++;
		if (i < l)
		{
			unsigned char b1 = (rstr[(int)input[i]] << 2 & 0xfc) +
					(rstr[(int)input[i + 1]] >> 4 & 0x03);
			if (output)
			{
				output[j] = b1;
			}
			j++;
			if (input[i + 2] != '=')
			{
				unsigned char b2 = (rstr[(int)input[i + 1]] << 4 & 0xf0) +
						(rstr[(int)input[i + 2]] >> 2 & 0x0f);
				if (output)
				{
					output[j] = b2;
				}
				j++;
			}
			if (input[i + 3] != '=')
			{
				unsigned char b3 = (rstr[(int)input[i + 2]] << 6 & 0xc0) +
						rstr[(int)input[i + 3]];
				if (output)
				{
					output[j] = b3;
				}
				j++;
			}
			i += 4;
		}
	}
	sz = j;
}


