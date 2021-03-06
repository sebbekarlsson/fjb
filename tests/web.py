import os
import subprocess
import time
from selenium import webdriver
from selenium.webdriver.chrome.options import Options

options = Options()
options.add_argument("--headless")
options.add_argument("--disable-extensions")

driver = webdriver.Chrome(
    executable_path=os.path.abspath("./driver/chromedriver"), options=options
)

pages = [
    dict(
        entry="index.jsx",
        url="./fjb-samples/jsx",
        f=lambda x: len(x.find_elements_by_tag_name('div')) >= 66
    ),
    dict(
        entry="index.jsx",
        url="./fjb-samples/with_react",
        text="simple",
        f=lambda x: x.find_element_by_id('root')
    ),
    dict(
        entry="index.ts",
        url="./fjb-samples/ts_vue",
        text='Hello there'
    )
]

for page in pages:
    entry = page['entry']
    p = os.path.abspath(page['url'])
    res = subprocess.run(
        f"cd {p} && yarn install", shell=True, capture_output=True)

    print(res.stdout, res.stderr)

    res = subprocess.run(
        f"cd {p} && ../../../fjb.out {entry} -o dist.js",
        shell=True,
        capture_output=True)

    print(res.stdout, res.stderr)

    url = f'file://{p}/index.html'
    print(url)
    driver.get(url)

    time.sleep(1)

    assert driver.find_element_by_tag_name('body')

    if 'f' in page:
        assert page['f'](driver)

    if 'text' in page:
        if page['text']:
            print(page['text'])
            assert page['text'] in driver.find_element_by_tag_name('body').text
