var gulp = require('gulp'),
    connect = require('gulp-connect'),
    inline = require('gulp-inline'),
    uglify = require('gulp-uglify'),
    minify = require('gulp-minify-html');

gulp.task('dist', function() {
    gulp.src('app/index.html')
        .pipe(inline({
            base: 'app/',
            js: uglify()
        }))
        .pipe(minify())
        .pipe(gulp.dest('dist/'));

    connect.server({
        root: 'dist/'
    });
});

gulp.task('webserver', function() {
    connect.server({
        root: 'app'
    });
});

gulp.task('default', ['webserver']);
