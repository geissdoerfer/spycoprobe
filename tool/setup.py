from setuptools import setup
import pathlib

# The directory containing this file
HERE = pathlib.Path(__file__).parent

# The text of the README file
README = (HERE.parent / "README.md").read_text()

setup(
    name="spycoprobe",
    version="1.0.0",
    description="Tool for controlling Spycoprobe SBW debugger",
    long_description=README,
    long_description_content_type="text/markdown",
    author="Kai Geissdoerfer",
    packages=["spycoprobe"],
    license="MIT",
    include_package_data=True,
    install_requires=[
        "numpy",
        "intelhex",
        "click",
        "pyserial",
    ],
    tests_require=["pytest"],
    entry_points={"console_scripts": ["spycoprobe=spycoprobe:cli"]},
)
