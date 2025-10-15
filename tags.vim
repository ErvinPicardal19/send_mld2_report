command! MakeTags execute '!ctags -R --links=yes --languages=C,C++,Make --c-kinds=+p --c++-kinds=+p --fields=+iaS --extras=+q -I__attribute__,__inline__ -I __asm__ -I __volatile__ -I restrict ' .
    \ '-f ' . g:project_root . '/.tags /usr/include /usr/local/include '
    \ . getcwd()

execute 'set tags+=' . fnameescape(g:project_root . '/.tags')
