function! BuildIncludeFlags(root) abort
    let l:root = substitute(a:root, '//\+', '/', 'g')
    let l:includes = systemlist('find ' . shellescape(l:root) . ' -type d')
    return join(map(l:includes, {_, val -> '-I' . val}), ' ')
endfunction

let g:ale_c_cc_options .= BuildIncludeFlags(g:project_root . '/lib') . ' '
let g:ale_c_cc_options .= BuildIncludeFlags(g:project_root . '/src') . ' '
"let g:ale_c_cc_options .= getcwd())
