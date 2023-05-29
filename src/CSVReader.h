
#ifndef CITYNETWORK_CSVREADER_H
#define CITYNETWORK_CSVREADER_H

#include <vector>
#include <string>

typedef std::vector<std::string> CSVLine;
typedef std::vector<CSVLine> CSV;

/**
 * The CSVReader namespace groups up all functions that have to do with reading and csv file.
 */
namespace CSVReader {
    /**
     * Reads any csv file given the file path.
     * @param const std::string& file
     * @return CSV aka std::vector<std::vector<std::string>>
     */
    CSV read(const std::string& file);
}

#endif //CITYNETWORK_CSVREADER_H
