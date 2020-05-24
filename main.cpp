#include <bits/stdc++.h>
using namespace std;

long long number_of_files(vector<string> &file_name_list, string file_name)
{
    long long count = 0;
    string temp;
    ifstream no_of_files("./data/" + file_name);
    while (no_of_files >> temp)
    {
        file_name_list.push_back(temp);
        ++count;
    }

    no_of_files.close();
    return count;
}

double idf_word(string word, vector<vector<string>> words_in_each_file, long long total_no_of_files)
{
    long long documents_with_word = 0;
    for (long long i = 0; i < total_no_of_files; ++i)
    {
        if (binary_search(words_in_each_file[i].begin(), words_in_each_file[i].end(), word) == true)
            ++documents_with_word;
    }

    if (documents_with_word == 0)
        return 1;
    else
        return 1 + log(double(total_no_of_files) / double(documents_with_word));
}

void processing_input_documents(vector<map<string, double>> &normalized_tf_documents, vector<vector<string>> &words_in_each_file, set<string> &total_unique_words, string file_name)
{
    string files;
    long long current_file = 0;
    ifstream file_with_names("./data/" + file_name);

    while (file_with_names >> files)
    {
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
}

vector<vector<double>> tf_idf_doc(vector<map<string, double>> normalized_tf_documents, map<string, double> idf, vector<string> given_words, long long total_no_of_files)
{
    long long total_given_words = given_words.size();

    vector<vector<double>> tf_idf_documents(total_given_words, vector<double>(total_no_of_files, 0));
    for (long long i = 0; i < total_given_words; ++i)
    {
        string current_word_from_input = given_words[i];
        for (long long j = 0; j < total_no_of_files; ++j)
            tf_idf_documents[i][j] = normalized_tf_documents[j][current_word_from_input] * idf[current_word_from_input];
    }

    return tf_idf_documents;
}

vector<double> cosine_similarity(vector<vector<double>> tf_idf_documents, vector<double> tf_idf_input, long long total_no_of_files, long long total_given_words)
{
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

    return final_scores;
}

int main(int argc, char* argv[])
{
    if (argc == 1)
        cout << "Please Enter Filename and Query" << endl;
    else if (argc == 2)
        cout << "Please Enter Query" << endl;
    else
    {   
        vector<string> given_words;
        for (long long i = 2; i < argc; ++i)
            given_words.push_back(argv[i]);

        long long total_given_words = argc - 2;

        string file_name = argv[1];
        vector<string> file_name_list;
        long long total_no_of_files = number_of_files(file_name_list, file_name);

        vector<map<string, double>> normalized_tf_documents;
        vector<vector<string>> words_in_each_file;
        set<string> total_unique_words;

        processing_input_documents(normalized_tf_documents, words_in_each_file, total_unique_words, file_name);

        map<string, double> inverse_document_frequency;
        for (auto i : total_unique_words)
            inverse_document_frequency[i] = idf_word(i, words_in_each_file, total_no_of_files);

        vector<vector<double>> tf_idf_documents = tf_idf_doc(normalized_tf_documents, inverse_document_frequency, given_words, total_no_of_files);

        map<string, double> tf_input;
        for (long long i = 0; i < total_given_words; ++i)
            tf_input[given_words[i]] += 1;

        map<string, double> normalized_tf_input;
        map<string, double>:: iterator input_words;
        for (input_words = tf_input.begin(); input_words != tf_input.end(); ++input_words)
            normalized_tf_input[input_words->first] = input_words->second / double(total_given_words);

        vector<double> tf_idf_input(total_given_words);
        for (long long i = 0; i < total_given_words; ++i)
        {
            string current_word_from_input = given_words[i];
            tf_idf_input[i] = normalized_tf_input[current_word_from_input] * inverse_document_frequency[current_word_from_input];
        }

        vector<double> final_scores = cosine_similarity(tf_idf_documents, tf_idf_input, total_no_of_files, total_given_words);

        double max_value = *max_element(final_scores.begin(), final_scores.end());
        cout << "Relevant Documents : " << endl;
        for (long long i = 0; i < total_no_of_files; ++i)
        {
            if (float(final_scores[i]) == float(max_value))
                cout << file_name_list[i] << endl;
        }
    }
    
    return 0;
}
