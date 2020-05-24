#include <bits/stdc++.h>
using namespace std;

long long get_number_of_files(string file_name)
{
    long long count = 0;
    string temp;
    ifstream no_of_files("./data/" + file_name);
    while (no_of_files >> temp)
        ++count;

    no_of_files.close();
    return count;
}

double idf(string term, vector<vector<string>> s)
{
    long long num_documents_with_term = 0;
    for (long long i = 0; i < s.size(); ++i)
    {
        if (binary_search(s[i].begin(), s[i].end(), term) == true)
            ++num_documents_with_term;
    }

    if (num_documents_with_term == 0)
        return 1;
    else
        return 1 + log(double(s.size()) / double(num_documents_with_term));
}

int main(int argc, char* argv[])
{
    if (argc == 1)
        cout << "Please Enter Filename and Query" << endl;
    else if (argc == 2)
        cout << "Please Enter Query" << endl;
    else
    {   
        vector<string> terms;
        for (long long i = 2; i < argc; ++i)
            terms.push_back(argv[i]);

        long long total_given_words = argc - 2;

        string file_name = argv[1];
        long long total_no_of_files = get_number_of_files(file_name);
        vector<string> file_names(total_no_of_files);

        vector<map<string, double>> normalized_tf_documents;
        vector<vector<string>> words_in_each_file;
        set<string> total_unique_words;

        string files;
        long long current_file = 0;
        ifstream file_with_names("./data/" + file_name);

        while (file_with_names >> files)
        {
            file_names[current_file] = files;

            vector<string> words_in_current_file;
            map<string, double> current_file_words;
            string words;
            long long total_words_in_file = 0;
            ifstream each_file("./data/" + files);

            while (each_file >> words)
            {
                current_file_words[words] += 1;
                ++total_words_in_file;

                total_unique_words.insert(words);
                words_in_current_file.push_back(words);
            }
            normalized_tf_documents.push_back(current_file_words);

            map<string, double>:: iterator itr;
            for (itr = normalized_tf_documents[current_file].begin(); itr != normalized_tf_documents[current_file].end(); ++itr)
            {
                double count = itr->second;
                itr->second = count / double(total_words_in_file);
            }

            sort(words_in_current_file.begin(), words_in_current_file.end());
            words_in_each_file.push_back(words_in_current_file);

            each_file.close();
            ++current_file;
        }
        file_with_names.close();

        map<string, double> inverse_document_frequency;
        for (auto i : total_unique_words)
            inverse_document_frequency[i] = idf(i, words_in_each_file);

        vector<vector<double>> tf_idf_documents(total_given_words, vector<double>(total_no_of_files, 0));
        for (long long i = 0; i < total_given_words; ++i)
        {
            string current_word_from_input = terms[i];
            for (long long j = 0; j < total_no_of_files; ++j)
                tf_idf_documents[i][j] = normalized_tf_documents[j][current_word_from_input] * inverse_document_frequency[current_word_from_input];
        }

        map<string, double> normalized_tf_input;
        for (long long i = 0; i < total_given_words; ++i)
            normalized_tf_input[terms[i]] += 1;

        map<string, double>:: iterator input_words;
        for (input_words = normalized_tf_input.begin(); input_words != normalized_tf_input.end(); ++input_words)
        {
            double count = input_words->second;
            input_words->second = count / double(total_given_words);
        }

        vector<double> tf_idf_input(total_given_words);
        for (long long i = 0; i < total_given_words; ++i)
        {
            string current_word_from_input = terms[i];
            tf_idf_input[i] = normalized_tf_input[current_word_from_input] * inverse_document_frequency[current_word_from_input];
        }

        vector<double> final_scores(total_no_of_files, 0);
        for (long long i = 0; i < total_no_of_files; ++i)
        {
            double magnitude_document = 0, magnitude_query = 0;
            for (long long j = 0; j < total_given_words; ++j)
            {
                final_scores[i] += (tf_idf_documents[j][i] * tf_idf_input[j]);
                magnitude_document += (tf_idf_documents[j][i] * tf_idf_documents[j][i]);
                magnitude_query += (tf_idf_input[j] * tf_idf_input[j]);
            }

            if (magnitude_document != 0)
                final_scores[i] = final_scores[i] / (sqrt(magnitude_document) * sqrt(magnitude_query)); 
        }

        double max_value = *max_element(final_scores.begin(), final_scores.end());
        cout << "Relevant Documents : " << endl;
        for (long long i = 0; i < total_no_of_files; ++i)
        {
            if (float(final_scores[i]) == float(max_value))
                cout << file_names[i] << endl;
        }
    }
    
    return 0;
}
