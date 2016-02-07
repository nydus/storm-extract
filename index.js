var bindings = require('bindings')('storm-extract');

module.exports = {
    getVersion: function() {
        return bindings.getVersion();
    },

    extractFiles: function(Source, Destination, Files) {
        return bindings.extractFiles(Source, Destination, Files);
    },

    listFiles: function(Directory) {
        return bindings.listFiles(Directory);
    }
};
