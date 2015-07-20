var gulp = require('gulp'),
	connect = require('gulp-connect');

gulp.task('webserver', function() {
    connect.server({
    	root: 'app'
    });
});

gulp.task('default', ['webserver']);
