{
    'targets': [{
        'target_name': 'storm-extract'
      , 'defines' : [
        'NODE=true'
      ]
      , 'include_dirs' : [
            '<!(node -e "require(\'nan\')")',
            'include/'
            'CascLib/src/'
        ]
      , 'dependencies': [
            '<(module_root_dir)/CascLib.gyp:CascLib'
        ]
      , 'sources': [
            'src/storm-extract.cpp'
        ]
    }]
}