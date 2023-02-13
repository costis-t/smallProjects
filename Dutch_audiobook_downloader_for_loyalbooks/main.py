import os
import argparse
import requests
from bs4 import BeautifulSoup
from typing import List

# Command
class DownloadCommand:
    def __init__(self, url: str, subfolder: str, book_author: str, book_title: str, filename: str):
        self.url = url
        self.subfolder = subfolder
        self.book_title = book_title
        self.book_author = book_author
        self.filename = filename
    
    def execute(self):
        file_path = os.path.join(self.subfolder, self.filename)
        wget_command = f"wget  -O \"{self.subfolder}/{self.book_author}_-_{self.book_title}_::_{self.filename}\" {self.url}"
        with open('wget_commands.bash', 'a') as f:
            f.write(wget_command + "\n")
        print(f"Writing {self.url}")

class LoyalBooksCrawler:
    def __init__(self, urls: List[str], subfolder: str = 'downloaded_audiobooks'):
        self.urls = urls
        self.subfolder = subfolder
    
    def create_download_folder(self):
        if not os.path.exists(self.subfolder):
            os.mkdir(self.subfolder)
    
    def create_wget_command_file(self):
        with open('wget_commands.bash', 'w') as f:
            f.write("#! /bin/bash\n")
    
    def get_book_links(self, url: str) -> List[str]:
        res = requests.get(url)
        soup = BeautifulSoup(res.content, 'html.parser')
        book_links = [link['href'] for link in soup.find_all('a', href=True) if 'book/' in link['href']]
        return book_links
    
    def get_book_info(self, book_link: str):
        res = requests.get(f"http://www.loyalbooks.com/{book_link}")
        soup = BeautifulSoup(res.content, 'html.parser')
        book_title = soup.find('span', {'itemprop': 'name'}).text
        try:
            book_author = soup.findAll('a', {'itemprop': 'author'})[0].text 
        except:
            book_author = soup.find('span', {'itemprop': 'author'}).text # pfff the first doesn't work in some
        mp3_download_link = soup.findAll('a', {'class': 'download'})[0]['href']
        filename = mp3_download_link.split('/')[-1]
        return (book_author, book_title, mp3_download_link, filename)
    
    def run(self):
        self.create_download_folder()
        self.create_wget_command_file()
        for url in self.urls:
            book_links = self.get_book_links(url)
            for book_link in set(book_links):
                book_title, book_author, mp3_download_link, filename = self.get_book_info(book_link)
                download_command = DownloadCommand(mp3_download_link, self.subfolder, book_title, book_author, filename)
                download_command.execute()

if __name__ == '__main__':
    parser = argparse.ArgumentParser(description='LoyalBooks audiobook downloader')
    parser.add_argument('language', type=str, help='Language of audiobooks')
    parser.add_argument('pages', type=int, help='Pages to crawl')
    args = parser.parse_args()
    urls = [f"http://www.loyalbooks.com/language/{args.language}?page={i}" for i in range(1, args.pages + 1)]
    crawler = LoyalBooksCrawler(urls)
    crawler.run()
