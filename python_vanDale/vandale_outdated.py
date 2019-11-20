#!/usr/bin/python

from bs4 import BeautifulSoup
import urllib.request
import requests
import re
import glob
from googletrans import Translator

translator = Translator()

wordlist_files = glob.glob('sample.wordlist')
for filename in wordlist_files:
    tex_filename = re.sub(".wordlist", ".tex", filename)
    word_list = [word for line in open(filename, 'r') for word in line.split()]
    f = open(tex_filename, 'w')
    f.write(f"\section{{Words for {filename}}}\n")
    f.write(f"\n")
    f.write(f"\\begin{{multicols}}{{2}}\n")
    f.write(f"\\begin{{enumerate}}\n")
    for word in word_list:
        print(word)
        url_nederlands = "https://www.vandale.nl/gratis-woordenboek/nederlands/betekenis/" + word
        response_nederlands = requests.get(url_nederlands)
        soup_nederlands = BeautifulSoup(response_nederlands.text)
        my_soup_nederlands = soup_nederlands.find('div', {'class': 'snippets'}).find('span')
        
        url_english = "https://www.vandale.nl/gratis-woordenboek/nederlands-engels/vertaling/" + word
        response_english = requests.get(url_english)
        soup_english = BeautifulSoup(response_english.text)
        my_soup_english = soup_english.find('div', {'class': 'snippets'}).find('span')
        
        if my_soup_english is None:
            f.write(f"\\item \\parbox[t][][t]{{35mm}}{{\\textbf{{{word}}}\\\\\\href{{https://www.vandale.nl/gratis-woordenboek/nederlands/betekenis/{word}}}{{\\tiny NL}}{{\\tiny, }}\\href{{https://www.vandale.nl/gratis-woordenboek/nederlands-engels/vertaling/{word}}}{{\\tiny EN}}{{\\tiny, }}\\href{{https://www.wordreference.com/nlen/{word}}}{{\\tiny WR}}{{\\tiny, }}\\href{{https://translate.google.com/\\#view=home\\&op=translate\\&sl=nl\\&tl=en\\&text={word}}}{{\\tiny G}}}} \\parbox[t][][t]{{55mm}}{{{translator.translate(word, src='nl', dest='en').text} {{\\tiny (from google)}}}}\n")
            print(f"\\item \\parbox[t][][t]{{35mm}}{{\\textbf{{{word}}}\\\\\\href{{https://www.vandale.nl/gratis-woordenboek/nederlands/betekenis/{word}}}{{\\tiny NL}}{{\\tiny, }}\\href{{https://www.vandale.nl/gratis-woordenboek/nederlands-engels/vertaling/{word}}}{{\\tiny EN}}{{\\tiny, }}\\href{{https://www.wordreference.com/nlen/{word}}}{{\\tiny WR}}{{\\tiny, }}\\href{{https://translate.google.com/\\#view=home\\&op=translate\\&sl=nl\\&tl=en\\&text={word}}}{{\\tiny G}}}} \\parbox[t][][t]{{55mm}}{{{translator.translate(word, src='nl', dest='en').text} {{\\tiny (from google)}}}}\n")
        # neveneffect has no nederlands text, only english (!), hence the elif
        elif my_soup_nederlands is None: 
            bold_word = word
            #rest_first_line = re.search('\s\d(.+?$)', my_soup_english.text).group(1) # problem with bouwwerf
            first_line_with_potential_space_in_beginning = re.search('\d(.+?$)', my_soup_english.text).group(1)
            rest_first_line = first_line_with_potential_space_in_beginning.lstrip(' ') # remove leading spaces
            f.write(f"\\item \\parbox[t][][t]{{35mm}}{{\\textbf{{{bold_word}}}\\\\\\href{{https://www.vandale.nl/gratis-woordenboek/nederlands/betekenis/{word}}}{{\\tiny NL}}{{\\tiny, }}\\href{{https://www.vandale.nl/gratis-woordenboek/nederlands-engels/vertaling/{word}}}{{\\tiny EN}}{{\\tiny, }}\\href{{https://www.wordreference.com/nlen/{word}}}{{\\tiny WR}}{{\\tiny, }}\\href{{https://translate.google.com/\\#view=home\\&op=translate\\&sl=nl\\&tl=en\\&text={word}}}{{\\tiny G}}}} \\parbox[t][][t]{{55mm}}{{{rest_first_line[0:61]}\\ldots}}\n")
            print(f"\\item \\parbox[t][][t]{{35mm}}{{\\textbf{{{bold_word}}}\\\\\\href{{https://www.vandale.nl/gratis-woordenboek/nederlands/betekenis/{word}}}{{\\tiny NL}}{{\\tiny, }}\\href{{https://www.vandale.nl/gratis-woordenboek/nederlands-engels/vertaling/{word}}}{{\\tiny EN}}{{\\tiny, }}\\href{{https://www.wordreference.com/nlen/{word}}}{{\\tiny WR}}{{\\tiny, }}\\href{{https://translate.google.com/\\#view=home\\&op=translate\\&sl=nl\\&tl=en\\&text={word}}}{{\\tiny G}}}} \\parbox[t][][t]{{55mm}}{{{rest_first_line[0:61]}\\ldots}}\n")
        else:
            # heleboel has no explanation in parenthesis
            if bool(re.search(r'\(',  my_soup_nederlands.text)) is True:
                        first_line_with_numbers = re.search('(^.+?\))\s\d', my_soup_nederlands.text).group(1)
                        first_line = re.sub('\d+', '', first_line_with_numbers)
                        bold_word = re.search('(^.+?)\s\(', first_line).group(1)
                        rest_first_line = '(' + re.search('\s\((.+?$)', first_line).group(1)
                        # tillen has multiple meaning, I keep the first which might not be correct
                        if bool(re.search(r'\d',  my_soup_english.text)) is False:
                            second_line = re.search('\s(.+?$)', my_soup_english.text).group(1)
                        else:
                            # if has multiple meanings, keep the first
                            if bool(re.search(r'2',  my_soup_english.text)) is True:
                                second_line = re.search('\s1(.+?)2', my_soup_english.text).group(1)
                                f.write(f"\\item \\parbox[t][][t]{{35mm}}{{\\textbf{{{bold_word}}}\\\\\\href{{https://www.vandale.nl/gratis-woordenboek/nederlands/betekenis/{word}}}{{\\tiny NL}}{{\\tiny, }}\\href{{https://www.vandale.nl/gratis-woordenboek/nederlands-engels/vertaling/{word}}}{{\\tiny EN}}{{\\tiny, }}\\href{{https://www.wordreference.com/nlen/{word}}}{{\\tiny WR}}{{\\tiny, }}\\href{{https://translate.google.com/\\#view=home\\&op=translate\\&sl=nl\\&tl=en\\&text={word}}}{{\\tiny G}}}} \\parbox[t][][t]{{55mm}}{{{rest_first_line}\\\\{second_line[0:61]}\\ldots}}\n")
                                print(f"\\item \\parbox[t][][t]{{35mm}}{{\\textbf{{{bold_word}}}\\\\\\href{{https://www.vandale.nl/gratis-woordenboek/nederlands/betekenis/{word}}}{{\\tiny NL}}{{\\tiny, }}\\href{{https://www.vandale.nl/gratis-woordenboek/nederlands-engels/vertaling/{word}}}{{\\tiny EN}}{{\\tiny, }}\\href{{https://www.wordreference.com/nlen/{word}}}{{\\tiny WR}}{{\\tiny, }}\\href{{https://translate.google.com/\\#view=home\\&op=translate\\&sl=nl\\&tl=en\\&text={word}}}{{\\tiny G}}}} \\parbox[t][][t]{{55mm}}{{{rest_first_line}\\\\{second_line[0:61]}\\ldots}}\n")
                            else:
                                second_line = re.sub('\d+', '', re.search('\d(.+?$)', my_soup_english.text).group(1))
                                f.write(f"\\item \\parbox[t][][t]{{35mm}}{{\\textbf{{{bold_word}}}\\\\\\href{{https://www.vandale.nl/gratis-woordenboek/nederlands/betekenis/{word}}}{{\\tiny NL}}{{\\tiny, }}\\href{{https://www.vandale.nl/gratis-woordenboek/nederlands-engels/vertaling/{word}}}{{\\tiny EN}}{{\\tiny, }}\\href{{https://www.wordreference.com/nlen/{word}}}{{\\tiny WR}}{{\\tiny, }}\\href{{https://translate.google.com/\\#view=home\\&op=translate\\&sl=nl\\&tl=en\\&text={word}}}{{\\tiny G}}}} \\parbox[t][][t]{{55mm}}{{{rest_first_line}\\\\{second_line[0:61]}\\ldots}}\n")
                                print(f"\\item \\parbox[t][][t]{{35mm}}{{\\textbf{{{bold_word}}}\\\\\\href{{https://www.vandale.nl/gratis-woordenboek/nederlands/betekenis/{word}}}{{\\tiny NL}}{{\\tiny, }}\\href{{https://www.vandale.nl/gratis-woordenboek/nederlands-engels/vertaling/{word}}}{{\\tiny EN}}{{\\tiny, }}\\href{{https://www.wordreference.com/nlen/{word}}}{{\\tiny WR}}{{\\tiny, }}\\href{{https://translate.google.csom/\\#view=home\\&op=translate\\&sl=nl\\&tl=en\\&text={word}}}{{\\tiny G}}}} \\parbox[t][][t]{{55mm}}{{{rest_first_line}\\\\{second_line[0:61]}\\ldots}}\n")
                                
            else:
                first_line_with_numbers = re.search('(^.+?)\s\d', my_soup_nederlands.text).group(1)
                first_line = re.sub('\d+', '', first_line_with_numbers)
                bold_word = re.search('(^.+?)$', first_line).group(1)
                rest_first_line = ''
                # tillen has multiple meaning, I keep the first which might not be correct
                if bool(re.search(r'\d',  my_soup_english.text)) is False:
                    second_line = re.search('\s(.+?$)', my_soup_english.text).group(1)
                else:
                    second_line = re.search('\d(.+?$)', my_soup_english.text).group(1)
                    f.write(f"\\item \\parbox[t][][t]{{35mm}}{{\\textbf{{{bold_word}}}\\\\\\href{{https://www.vandale.nl/gratis-woordenboek/nederlands/betekenis/{word}}}{{\\tiny NL}}{{\\tiny, }}\\href{{https://www.vandale.nl/gratis-woordenboek/nederlands-engels/vertaling/{word}}}{{\\tiny EN}}{{\\tiny, }}\\href{{https://www.wordreference.com/nlen/{word}}}{{\\tiny WR}}{{\\tiny, }}\\href{{https://translate.google.com/\\#view=home\\&op=translate\\&sl=nl\\&tl=en\\&text={word}}}{{\\tiny G}}}} \\parbox[t][][t]{{55mm}}{{{rest_first_line}\\\\{second_line[0:61]}\\ldots}}\n")
                    print(f"\\item \\parbox[t][][t]{{35mm}}{{\\textbf{{{bold_word}}}\\\\\\href{{https://www.vandale.nl/gratis-woordenboek/nederlands/betekenis/{word}}}{{\\tiny NL}}{{\\tiny, }}\\href{{https://www.vandale.nl/gratis-woordenboek/nederlands-engels/vertaling/{word}}}{{\\tiny EN}}{{\\tiny, }}\\href{{https://www.wordreference.com/nlen/{word}}}{{\\tiny WR}}{{\\tiny, }}\\href{{https://translate.google.com/\\#view=home\\&op=translate\\&sl=nl\\&tl=en\\&text={word}}}{{\\tiny G}}}} \\parbox[t][][t]{{55mm}}{{{rest_first_line}\\\\{second_line[0:61]}\\ldots}}\n")
                
    f.write(f"\\end{{enumerate}}\n")
    f.write(f"\\end{{multicols}}\n")
    f.close()
    

for filename in wordlist_files:
    tex_filename = re.sub(".wordlist", ".tex", filename)
    print(f"\input{{{tex_filename}}}")
