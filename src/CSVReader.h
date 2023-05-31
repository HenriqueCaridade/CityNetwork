
#ifndef CITYNETWORK_CSVREADER_H
#define CITYNETWORK_CSVREADER_H

#include <vector>
#include <string>

typedef std::vector<std::string> CSVLine;
typedef std::vector<CSVLine> CSV;

/**
 * @brief The CSVReader namespace groups functions related to reading CSV files.
 */
namespace CSVReader {
    /**
     * @brief Reads a CSV file from the given file path.
     *
     * This function reads the contents of a CSV file and returns it as a CSV structure,
     * which is a vector of vector of strings.
     *
     * @param file The path to the CSV file.
     * @return CSV The parsed CSV data.
     */
    CSV read(const std::string& file);
}

#endif // CITYNETWORK_CSVREADER_H