var webpack = require("webpack");

module.exports = {
    entry: {
        homePage: "./src/main/webapp/js/index.js"
    },

    output: {
        filename: "[name].bundle.js",
        path: __dirname + "/src/main/webapp/js/dist",
        chunkFilename: "[name].bundle.js",
        publicPath: "./js/dist/"
    },

    plugins: [
        new webpack.optimize.CommonsChunkPlugin({
            name: 'common'
        })
    ]
};