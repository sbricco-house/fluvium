
/**
 * Validate the mandatory arguments sent to lambda function
 * @param {Object} event - aws lambda event object
 * @param {...string} keys - array of key of mandatory fields.
 * @return {boolean} True if all field exists
 */
module.exports.validateMandatoryArgs = function(event, ...keys) {
    return keys.every(key => typeof(event[key]) !== 'undefined');
};