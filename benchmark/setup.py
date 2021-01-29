from setuptools import setup, find_packages


setup(
    name='fjb_bench',
    version='1.0.0',
    install_requires=[
        'jinja2'
    ],
    packages=find_packages(),
    entry_points={
        'console_scripts': [
        ]
    }
)
