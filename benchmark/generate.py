import subprocess
import jinja2
import os
import datetime

PWD = os.getcwd()

ESBUILD_PATH = "/benchmark/node_modules/.bin/esbuild"
ESBUILD_ARGS = ' --bundle --outfile=./dist.js'
FJB_PATH = "/fjb.out"
FJB_ARGS = ' > ./dist.js'


RUNS = [
    {
        'title': 'FJB',
        'exec': PWD + FJB_PATH,
        "args": FJB_ARGS
    },
    {
        'title': 'esbuild',
        'exec': PWD + ESBUILD_PATH,
        "args": ESBUILD_ARGS
    },
]


def create_mark(title, path, runs=RUNS):
    return {
        "title": title,
        "path": PWD + path,
        "runs": runs or RUNS
    }


BENCHMARKS = [
    create_mark(
        "Importing ES6 modules",
        '/tests/src/test_projects/es6/index.js'
    ),
    create_mark(
        "Importing aliased ES6 modules",
        '/tests/src/test_projects/alias_imports/index.js'
    ),
    create_mark(
        "Importing CSS",
        '/tests/src/test_projects/css_import/index.js'
    ),
    create_mark(
        "Importing JSON",
        '/tests/src/test_projects/json_import/index.js'
    ),
    create_mark(
        "Importing `ceil` from lodash, with tree shaking.",
        '/tests/src/test_projects/with_lodash/index.js'
    )
]


def get_sysinfo():
    process = subprocess.Popen(
        "lscpu",
        stdout=subprocess.PIPE,
        shell=True
    )
    return process.communicate()[0].decode()


def get_filesize(filename):
    return str(os.path.getsize(filename)) + ' bytes'


def run_entry(mark, entry):
    timebefore = datetime.datetime.now()

    cmd = "{exe} {path} {args}".format(
            exe=entry['exec'], path=mark['path'], args=entry['args'])

    process = subprocess.Popen(
        cmd,
        stdout=subprocess.PIPE,
        shell=True
    )
    process.communicate()

    diff = str(datetime.datetime.now() - timebefore)

    return {
        'time': diff,
        'size': get_filesize(PWD + '/dist.js'),
        'cmd': cmd,
        **entry
    }


def run_benchmark(mark):
    return list(map(lambda x: run_entry(mark, x), mark['runs']))


def run_benchmarks(marks):
    return map(
        lambda x: dict(list(x.items()) + list(dict(
            runs=run_benchmark(x),
            source=open(x['path']).read(),
            sysinfo=get_sysinfo(),
        ).items())),
        marks
    )


def generate(results):
    template = jinja2.Template(open(
        PWD + '/benchmark/templates/mark.md').read())

    marks = list(results)
    contents = template.render(marks=marks, now=str(datetime.datetime.now()))
    open(os.getcwd() + '/benchmarks.md', 'w+').write(contents)


def run():
    generate(run_benchmarks(BENCHMARKS))


if __name__ == '__main__':
    run()
