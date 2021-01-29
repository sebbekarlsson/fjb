import subprocess
import jinja2
import os
import datetime

BENCHMARKS = [
    {
        "title": "Importing `ceil` from lodash, with tree shaking. (FJB)",
        "exec": os.getcwd() + "/fjb.out",
        "path": os.getcwd() + '/tests/src/test_projects/with_lodash/index.js',
        "args": " > ./dist.js"
    },
    {
        "title": "Importing `ceil` from lodash, with tree shaking. (ESBUILD)",
        "exec": os.getcwd() +
        "/benchmark/node_modules/.bin/esbuild",
        "path": os.getcwd() + '/tests/src/test_projects/with_lodash/index.js',
        "args": " --bundle --outfile=./dist.js"
    },
    {
        "title": "Simple project with es6 imports (FJB)",
        "exec": os.getcwd() + "/fjb.out",
        "path": os.getcwd() + '/tests/src/test_projects/es6/index.js',
        "args": " > ./dist.js"
    },
    {
        "title": "Simple project with es6 imports (ESBUILD)",
        "exec": os.getcwd() + "/benchmark/node_modules/.bin/esbuild",
        "path": os.getcwd() + '/tests/src/test_projects/es6/index.js',
        "args": " --bundle --outfile=./dist.js"
    }
]


def run_entry(exe, path, args):
    timebefore = datetime.datetime.now()

    process = subprocess.Popen(
        "{exe} {fpath} {args}".format(exe=exe, fpath=path, args=args),
        stdout=subprocess.PIPE,
        shell=True
    )
    process.communicate()

    diff = (datetime.datetime.now() - timebefore)

    return str(diff)


def get_sysinfo():
    process = subprocess.Popen(
        "lscpu",
        stdout=subprocess.PIPE,
        shell=True
    )
    return process.communicate()[0].decode()


def get_filesize(filename):
    process = subprocess.Popen(
        "du -h {}".format(filename),
        stdout=subprocess.PIPE,
        shell=True
    )
    return process.communicate()[0].decode()


def run_benchmark(mark):
    return {
        'mark': mark, 'result': run_entry(
            mark['exec'], mark['path'], mark['args']),
        'cmd': '{exe} {fpath}'.format(
            exe=os.path.basename(mark['exec']),
            fpath=os.path.basename(mark['path']),
        ),
        'source': open(mark['path']).read(),
        'outsize': get_filesize(os.getcwd() + '/dist.js')
    }


def run_benchmarks(marks):
    return map(run_benchmark, marks)


def generate(results):
    sysinfo = get_sysinfo()

    contents = ''

    for res in results:
        template = jinja2.Template(
            open(os.getcwd() + "/benchmark/templates/mark.md").read()
        )

        contents += template.render(
            sysinfo=sysinfo,
            result=res['result'],
            mark=res['mark'],
            cmd=res['cmd'],
            source=res['source'],
            outsize=res['outsize']
        ) + '\n\n'

    open(os.getcwd() + '/benchmarks.md', 'w+').write(contents)


def run():
    generate(run_benchmarks(BENCHMARKS))


if __name__ == '__main__':
    run()
