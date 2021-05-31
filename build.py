import os
import glob

getmtime  = os.path.getmtime
basename  = os.path.basename

extensions = ['c', 'cpp', 'rc']

target      = 'Tekisuto.exe'
target_time = getmtime(target) if os.path.exists(target) else 0

src_dir   = 'src'
obj_dir   = 'objects'

c_flags   = '-O1'
cxx_flags = c_flags + ' -std=gnu++2a -Wno-psabi'
ld_flags  = '-Wl,--gc-sections -mwindows -static-libgcc'

src_flags = \
 {
   'c' : c_flags,
   'cpp' : cxx_flags,
   'rc' : ''
 }

compiler  = \
 {
   'c' : 'gcc',
   'cpp' : 'g++',
   'rc' : 'windres'
 }

src_files = [ ]
obj_files = [ ]

updated   = False

def list2str(L):
  return " ".join(L)

def change_ext(name, x):
  return name[:name.find('.')] + '.' + x

def get_ext(name):
  return name[name.find('.') + 1:]

def get_depends(file):
  D = f'{obj_dir}/{basename(change_ext(file, "d"))}'
  res = os.system(f'gcc -MM -o {D} {file}')
  
  if res != 0:
    exit(0)
  
  ret = [ ]
  lines = open(D).readlines()
  
  for i in lines:
    i = i[i.find(':') + 2:]
    i = i.replace('\n', '')
    i = i.replace(' \\', '')
    i = i.split(' ')
    ret += i
  
  os.system(f'rm {D}')
  return ret

def check_build(file):
  obj = f'{obj_dir}/{basename(change_ext(file, "o"))}'
  obj_time = 0
  
  if not os.path.exists(obj):
    return True
  else:
    obj_time = getmtime(obj)
  
  depends = [ ]
  
  if get_ext(file) != 'rc':
    depends = get_depends(file)
  else:
    depends = [ file ]
  
  for D in depends:
    if getmtime(D) > obj_time:
      return True
  
  return target_time < obj_time

if not os.path.isdir(obj_dir):
  os.system(f'mkdir {obj_dir}')

for x in extensions:
  src_files += glob.glob(f'{src_dir}/**/*.{x}', recursive=True)

for src in src_files:
  if check_build(src):
    updated = True
    print(basename(src))
    
    if get_ext(src) == 'rc':
      res = os.system(f'windres {src} {obj_dir}/{basename(change_ext(src, "o"))}')
    else:
      res = os.system(f'{compiler[get_ext(src)]} {src_flags[get_ext(src)]} -c -o {obj_dir}/{basename(change_ext(src, "o"))} {src}')
    
    if res != 0:
      exit(1)

obj_files = [ obj_dir + '/' + basename(change_ext(src, 'o')) for src in src_files ]

if updated or not os.path.exists(target):
  print('linking...')
  os.system(f'g++ {ld_flags} -o {target} {list2str(obj_files)}')
else:
  print('build is not needed')