import os
from selenium import webdriver
from selenium.webdriver.common.keys import Keys
from selenium.webdriver.chrome.options import Options

options = Options()
options.add_argument("--headless")
options.add_argument("--disable-extensions")
# options.add_argument("--disable-dev-shm-usage")
# options.add_argument("--no-sandbox")
# options.add_argument("--remote-debugging-port=9222")
# options.binary_location = os.path.abspath("./driver/chromedriver")

driver = webdriver.Chrome(
    executable_path=os.path.abspath("./driver/chromedriver"),
    options=options
)


pages = [
    dict(url="fjb-samples/with_react/index.html", element_id='root'),
    dict(url="fjb-samples/with_vue/index.html", text='hello')
]

for page in pages:
    p = os.path.abspath(page['url'])
    driver.get('file://' + p)

    if 'id' in page:
        search_field = driver.find_element_by_id(page['id'])
        assert search_field
    elif 'text' in page:
        assert page['text'] in driver.find_element_by_tag_name('body').text
