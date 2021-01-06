/* Majik 4 server
 * Copyright (C) 1998, 1999  Majik Development Team <majik@majik.netti.fi>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#define FN	"/doc/example/voc-test"
#define NUM_VOWELS		4
#define NUM_H_CONS      16
#define NUM_S_CONS		12
#define NUM_CONS 		NUM_H_CONS + NUM_S_CONS
#define NUM_LETTERS		NUM_VOCALS + NUM_CONS
#define NUM_ADDITS		NUM_S_CONS + NUM_VOWELS + 5
#define VOWEL			1
#define HC				2
#define SC				3

string *vowels = ({ "a", "e", "o", "u" });
string *hard_consonants = ({ "r", "r", "r", "r", "g", "g", "g", "g", "c", "c", "c", "c", "t", "t", "k", "k" });
string *soft_consonants = ({ "b", "b", "d", "d", "h", "w", "l", "l", "m", "m", "n", "n" });
string *consonants = hard_consonants + soft_consonants;
string *letters = vowels + consonants;
string *additives = soft_consonants + vowels + ({ "r", "r", "r", "'", "'" });

int
query_type(string str)
{
   if(member_array(str, hard_consonants) != -1)
	 return HC;
   else if(member_array(str, vowels) != -1)
	 return VOWEL;
   else if(member_array(str, soft_consonants) != -1)
	 return SC;
   else
	 return 0;
}
   
string
add_to_word(string str)
{
   int len = strlen(str);
   string add;   
   string curr;
   string prev;
   
   if(len == 1)
	 {
		if(query_type(str) == HC)
		  {
			 while(1)
			   {
				  add = additives[random(NUM_ADDITS)];
				  if(add == "r" && str != "r")
					{
					   return add;
					}
				  else
					{
					   if(query_type(add) != HC && add != "'")
						 return add;
					}
			   }
		  }
		else if(query_type(str) == SC)
		  return vowels[random(NUM_VOWELS)];
		else
		  {
			 while(1)
			   {
				  add = letters[random(NUM_LETTERS)];
				  if(query_type(add) != HC)
					return add;
			   }
		  }
	 }
   
   curr = str[len-1..len-1];
   prev = str[len-2..len-2];
   switch(query_type(curr))
	 { 
	  case VOWEL:
		while(1)
		  {
			 add = letters[random(NUM_LETTERS)];
			 if(query_type(add) == VOWEL)
			   {
				  if(query_type(prev) == HC && curr != add)
					return add;
			   }
			 else 
			   return add;
		  }				  
		break;
	  case HC:
		while(1)
		  {
			 add = additives[random(NUM_ADDITS)];
			 
			 if(query_type(add) == VOWEL)
			   return add;
			 
			 if(add == "'")
			   add = "r";
			 
			 if(add == "r")
			   {
				  if(query_type(prev) == VOWEL)
					return add;
			   }
			 else
			   {
				  if(query_type(prev) == VOWEL)
					return add;
			   }
		  }
		break;
	  case SC:
		if(query_type(prev) == SC || query_type(prev) == HC)
		  {
			 return vowels[random(NUM_VOWELS)];
		  }
		else
		  {
			 return additives[random(NUM_ADDITS)];
		  }
		break;
	  default:
		add = sprintf("%s%s%s", 
					  hard_consonants[random(NUM_H_CONS)],
					  vowels[random(NUM_VOWELS)],
					  consonants[random(NUM_CONS)]);
		break;
	 }
}

string
generate_word()
{
   int i;
   int len = d("1d8+1");
   string add;
   string str;
      
   str = letters[random(NUM_LETTERS)];
   for(i=0;i<len-1;i++)
	 str += add_to_word(str);
   
   len = strlen(str);
   i = query_type(str[len-1..len-1]);
   if(i == SC || i == HC)
	 str += vowels[random(NUM_VOWELS)];
   else if(i == VOWEL)
	 {
		while(1)
		  {
			 add = additives[random(NUM_ADDITS)];
			 if(add == "n" && str[len-1..len-1] == "a")
			   continue;
			 if(add == "m" && str[len-1..len-1] == "u")
			   continue;
			 if(add != str[len-1..len-1] && add != "'")
			   break;
		  }
		str += add;
	 }
   else
	 {
		while(1)
		  {
			 add = sprintf("%s%s%s", 
						   hard_consonants[random(NUM_H_CONS)],
						   vowels[random(NUM_VOWELS)],
						   soft_consonants[random(NUM_S_CONS)]);
			 if(add[1..2] != "um" && add[1..2] != "an")
			   break;
		  }
		str += add;
	 }
   return str;
}
   
int
main(string str)
{
   int i, j;
   int num_words;
   string word;
   string *words;
   
   if(!str || str == "")
	 num_words = 50;
   else
	 num_words = to_int(str);
   
   if(file_size(FN) > -1)
	 words = explode(read_file(FN, 0), "\n");
   else
	 words = ({ });
   for(i=0,j=num_words;i<j;i++)
	 {
		word = generate_word();
		if(member_array(word, words) == -1)
		  {
			 write_file(FN, word + "\n");
			 words += ({ word });
		  }
		else
		  --i;
	 }
   return 1;
}
