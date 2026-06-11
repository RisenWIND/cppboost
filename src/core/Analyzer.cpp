#include "core/Analyzer.h"
#include <algorithm>

bool Analyzer::checkAnswer(const shared_ptr<Question>& question, const vector<int>& userAnswers) {
    if (!question) return false;
    
    switch (question->type) {
        case SINGLE_CHOICE: {
            auto choiceQ = dynamic_pointer_cast<ChoiceQuestion>(question);
            if (!choiceQ || userAnswers.empty()) return false;
            return checkSingleChoiceAnswer(choiceQ, userAnswers[0]);
        }
        case MULTIPLE_CHOICE: {
            auto choiceQ = dynamic_pointer_cast<ChoiceQuestion>(question);
            if (!choiceQ) return false;
            return checkMultipleChoiceAnswer(choiceQ, userAnswers);
        }
        case JUDGMENT: {
            auto judgmentQ = dynamic_pointer_cast<JudgmentQuestion>(question);
            if (!judgmentQ || userAnswers.empty()) return false;
            return checkJudgmentAnswer(judgmentQ, userAnswers[0] == 1);
        }
        case CODE_ANALYSIS: {
            auto codeQ = dynamic_pointer_cast<CodeAnalysisQuestion>(question);
            if (!codeQ) return false;
            return checkCodeAnalysisAnswer(codeQ, userAnswers);
        }
        default:
            return false;
    }
}

bool Analyzer::checkSingleChoiceAnswer(const shared_ptr<ChoiceQuestion>& question, int userAnswer) {
    if (!question || question->correctAnswers.empty()) return false;
    return question->correctAnswers[0] == userAnswer;
}

bool Analyzer::checkMultipleChoiceAnswer(const shared_ptr<ChoiceQuestion>& question, const vector<int>& userAnswers) {
    if (!question) return false;
    
    vector<int> correct = question->correctAnswers;
    vector<int> user = userAnswers;
    
    sort(correct.begin(), correct.end());
    sort(user.begin(), user.end());
    
    return correct == user;
}

bool Analyzer::checkJudgmentAnswer(const shared_ptr<JudgmentQuestion>& question, bool userAnswer) {
    if (!question) return false;
    return question->correctAnswer == userAnswer;
}

bool Analyzer::checkCodeAnalysisAnswer(const shared_ptr<CodeAnalysisQuestion>& question, const vector<int>& userAnswers) {
    if (!question) return false;
    
    vector<int> correct = question->correctAnswers;
    vector<int> user = userAnswers;
    
    sort(correct.begin(), correct.end());
    sort(user.begin(), user.end());
    
    return correct == user;
}

vector<int> Analyzer::getCorrectAnswers(const shared_ptr<Question>& question) {
    vector<int> result;
    
    if (!question) return result;
    
    switch (question->type) {
        case SINGLE_CHOICE:
        case MULTIPLE_CHOICE: {
            auto choiceQ = dynamic_pointer_cast<ChoiceQuestion>(question);
            if (choiceQ) {
                result = choiceQ->correctAnswers;
            }
            break;
        }
        case JUDGMENT: {
            auto judgmentQ = dynamic_pointer_cast<JudgmentQuestion>(question);
            if (judgmentQ) {
                result.push_back(judgmentQ->correctAnswer ? 1 : 0);
            }
            break;
        }
        case CODE_ANALYSIS: {
            auto codeQ = dynamic_pointer_cast<CodeAnalysisQuestion>(question);
            if (codeQ) {
                result = codeQ->correctAnswers;
            }
            break;
        }
        default:
            break;
    }
    
    return result;
}

string Analyzer::getCorrectAnswerString(const shared_ptr<Question>& question) {
    vector<int> answers = getCorrectAnswers(question);
    
    if (answers.empty()) return "N/A";
    
    string result;
    for (int i = 0; i < answers.size(); i++) {
        if (i > 0) result += ", ";
        
        if (question->type == JUDGMENT) {
            result += (answers[i] == 1 ? "正确" : "错误");
        } else {
            result += char('A' + answers[i]);
        }
    }
    
    return result;
}

double Analyzer::calculateModuleCorrectRate(const ModuleProgress& progress) {
    if (progress.attemptCount == 0) return 0.0;
    return (double)progress.correctCount / progress.attemptCount * 100.0;
}

vector<int> Analyzer::findWeakModules(const vector<ModuleProgress>& allModules) {
    vector<int> weakModules;
    
    for (const auto& module : allModules) {
        double correctRate = calculateModuleCorrectRate(module);
        if (correctRate < 60.0 && module.attemptCount > 0) {
            weakModules.push_back(module.moduleId);
        }
    }
    
    return weakModules;
}

vector<int> Analyzer::findStrongModules(const vector<ModuleProgress>& allModules) {
    vector<int> strongModules;
    
    for (const auto& module : allModules) {
        double correctRate = calculateModuleCorrectRate(module);
        if (correctRate >= 80.0 && module.attemptCount > 0) {
            strongModules.push_back(module.moduleId);
        }
    }
    
    return strongModules;
}
