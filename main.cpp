#include <bits/stdc++.h>
#define show(x) cout << #x << ": " << x << endl
#define ll long long
#define fst first
#define snd second
#define pb push_back
#define mp make_pair
#define fori(ini, end, num) for (ll i = ini; i < end; i += num)
#define forj(ini, end, num) for (ll j = ini; j < end; j += num)
#define vll vector<ll>
#define pr pair<ll, ll>
#define um unordered_map
#define all(v) v.begin(), v.end()
#define allr(v) v.rbegin(), v.rend()
#define light ios_base::sync_with_stdio(false);
#define bolt cin.tie(NULL); cout.tie(NULL);
#define endl "\n"
#define mod 1000000007
using namespace std;

ll get_number_of_files()
{
    ll count = 0;
    string temp;
    ifstream no_of_files("./data/file_names.txt", ios :: in);
    while (no_of_files >> temp)
        ++count;

    return count;
}

double idf(string term, vector<vector<string>> s)
{
    ll num_documents_with_term = 0;
    fori (0, s.size(), 1)
    {
        if (binary_search(all(s[i]), term) == true)
            ++num_documents_with_term;
    }

    if (num_documents_with_term == 0)
        return 1;
    else
        return 1 + log(double(s.size()) / double(num_documents_with_term));
}

int main()
{   
    string s;

    cout << "Enter Query : " << endl;
    getline(cin, s);

    vector<string> terms;
    string temp = "";
    fori (0, s.size(), 1)
    {
        if (s[i] == ' ')
        {
            terms.pb(temp);
            temp = "";
        }
        else
            temp += s[i];
    }
    terms.pb(temp);

    ll total_given_words = terms.size();

    ll total_no_of_files = get_number_of_files();
    vector<string> file_names(total_no_of_files);

    vector<map<string, double>> normalized_tf_documents;
    vector<vector<string>> words_in_each_file;
    set<string> total_unique_words;

    string files;
    ll current_file = 0;
    ifstream file_with_names("./data/file_names.txt", ios :: in);

    while (file_with_names >> files)
    {
        file_names[current_file] = files;

        vector<string> words_in_current_file;
        map<string, double> current_file_words;
        string words;
        ll total_words_in_file = 0;
        ifstream each_file("./data/" + files, ios :: in);

        while (each_file >> words)
        {
            current_file_words[words] += 1;
            ++total_words_in_file;

            total_unique_words.insert(words);
            words_in_current_file.pb(words);
        }
        normalized_tf_documents.pb(current_file_words);

        map<string, double>:: iterator itr;
        for (itr = normalized_tf_documents[current_file].begin(); itr != normalized_tf_documents[current_file].end(); ++itr)
        {
            double count = itr->snd;
            itr->snd = count / double(total_words_in_file);
        }

        sort(all(words_in_current_file));
        words_in_each_file.pb(words_in_current_file);

        each_file.close();
        ++current_file;
    }
    file_with_names.close();

    map<string, double> inverse_document_frequency;
    for (auto i : total_unique_words)
        inverse_document_frequency[i] = idf(i, words_in_each_file);

    vector<vector<double>> tf_idf_documents(total_given_words, vector<double>(total_no_of_files, 0));
    fori (0, total_given_words, 1)
    {
        string current_word_from_input = terms[i];
        forj (0, total_no_of_files, 1)
            tf_idf_documents[i][j] = normalized_tf_documents[j][current_word_from_input] * inverse_document_frequency[current_word_from_input];
    }

    map<string, double> normalized_tf_input;
    fori (0, total_given_words, 1)
        normalized_tf_input[terms[i]] += 1;

    map<string, double>:: iterator input_words;
    for (input_words = normalized_tf_input.begin(); input_words != normalized_tf_input.end(); ++input_words)
    {
        double count = input_words->snd;
        input_words->snd = count / double(total_given_words);
    }

    vector<double> tf_idf_input(total_given_words);
    fori (0, total_given_words, 1)
    {
        string current_word_from_input = terms[i];
        tf_idf_input[i] = normalized_tf_input[current_word_from_input] * inverse_document_frequency[current_word_from_input];
    }

    vector<double> final_scores(total_no_of_files, 0);
    fori (0, total_no_of_files, 1)
    {
        double magnitude_document = 0, magnitude_query = 0;
        forj (0, total_given_words, 1)
        {
            final_scores[i] += (tf_idf_documents[j][i] * tf_idf_input[j]);
            magnitude_document += (tf_idf_documents[j][i] * tf_idf_documents[j][i]);
            magnitude_query += (tf_idf_input[j] * tf_idf_input[j]);
        }

        if (magnitude_document != 0)
            final_scores[i] = final_scores[i] / (sqrt(magnitude_document) * sqrt(magnitude_query)); 
    }

    double max_value = *max_element(all(final_scores));
    cout << "\nRelevant Documents : " << endl;
    fori (0, total_no_of_files, 1)
    {
        if (final_scores[i] == max_value)
            cout << file_names[i] << endl;
    }
    
    return 0;
}
