{
    'variables': {
        'project_root%': '<(DEPTH)',
    },
        'includes': [
	'gyp_build/common.gypi',
    ],

    'targets': [
    {
        'target_name': 'all',
        'type': 'none',
        'dependencies': [
            '<(project_root)/src/libsnet.gyp:*',
	#'<(project_root)/src/libsv/libsv.gyp:*',
	#'<(project_root)/test/sv_test.gyp:*',
             '<(project_root)/test/test.gyp:*'
        ],
    },
    ], # targets
}