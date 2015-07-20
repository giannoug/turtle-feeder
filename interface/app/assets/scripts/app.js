(function() {
    var app = angular.module('FeederApp', ['ngMaterial']);

    var esp8266 = 'http://192.168.1.111';

    app.controller('AppCtrl', function($scope, $http, $mdSidenav) {
        $scope.feed = function() {
            $http.post(esp8266 + '/api/feeder/sweep').
            success(function(data, status, headers, config) {
                console.log('done!');
            }).
            error(function(data, status, headers, config) {
                console.log('error');
            });
        };

        $scope.setPosition = function() {
            var values = "value=" + $scope.value;

            $http.post(esp8266 + '/api/feeder/move', values).
            success(function(data, status, headers, config) {
                console.log('done!');
            }).
            error(function(data, status, headers, config) {
                console.log('error');
            });
        };
    });
})();
